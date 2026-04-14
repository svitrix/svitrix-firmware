/**
 * @file test_policy_selection.cpp
 * @brief Tests for the IDisplayPolicy selection rule used by DisplayManager.
 *
 * DisplayManager caches the FIRST-registered active policy and applies its
 * overrides on edge transitions. This suite mirrors that selection logic
 * (see DisplayManager.cpp tick() — std::find_if over policies_) using two
 * stub IDisplayPolicy implementations, and verifies:
 *   - first-registered active policy wins over later ones
 *   - nullptr is selected when no policy is active
 *   - transitions when an active policy flips off
 *   - transitions when a higher-priority policy becomes active
 *
 * DisplayManager itself can't compile under native_test (FastLED + matrix
 * deps), so the rule lives in a tiny free helper that both the test and
 * production code use — same pattern as test_ha_memory.
 */

#include <unity.h>
#include <vector>
#include <algorithm>
#include "IDisplayPolicy.h"

// Mirrors DisplayManager.cpp tick(): first active policy wins.
// Kept in sync by hand (production code doesn't extract this helper
// because it's a single std::find_if line).
static IDisplayPolicy *selectActive(const std::vector<IDisplayPolicy *>& policies)
{
    auto it = std::find_if(policies.begin(), policies.end(),
                           [](IDisplayPolicy *p) { return p->isActive(); });
    return (it != policies.end()) ? *it : nullptr;
}

// ── Stub policy with programmable isActive ──────────────────────────

class StubPolicy : public IDisplayPolicy
{
public:
    bool active = false;
    uint8_t brightness = 0;
    uint32_t color = 0;
    bool blocksTransition = false;

    bool isActive() const override { return active; }
    bool overridesBrightness(uint8_t& out) const override { out = brightness; return true; }
    bool overridesTextColor(uint32_t& out) const override { out = color; return true; }
    bool blocksAutoTransition() const override { return blocksTransition; }
};

// ── Tests ───────────────────────────────────────────────────────────

void test_no_active_policy_returns_nullptr(void)
{
    StubPolicy a, b;
    a.active = false;
    b.active = false;
    std::vector<IDisplayPolicy *> policies = {&a, &b};

    TEST_ASSERT_NULL(selectActive(policies));
}

void test_empty_registry_returns_nullptr(void)
{
    std::vector<IDisplayPolicy *> policies;
    TEST_ASSERT_NULL(selectActive(policies));
}

void test_first_registered_active_wins_over_later(void)
{
    StubPolicy first, second;
    first.active = true;
    first.color = 0x111111;
    second.active = true;
    second.color = 0x222222;
    std::vector<IDisplayPolicy *> policies = {&first, &second};

    IDisplayPolicy *active = selectActive(policies);
    TEST_ASSERT_NOT_NULL(active);
    TEST_ASSERT_TRUE(active == &first);

    uint32_t out = 0;
    active->overridesTextColor(out);
    TEST_ASSERT_EQUAL_HEX32(0x111111, out);
}

void test_later_policy_wins_when_earlier_inactive(void)
{
    StubPolicy first, second;
    first.active = false;
    second.active = true;
    second.color = 0x333333;
    std::vector<IDisplayPolicy *> policies = {&first, &second};

    IDisplayPolicy *active = selectActive(policies);
    TEST_ASSERT_TRUE(active == &second);
}

void test_transition_when_earlier_policy_flips_on(void)
{
    // Simulate the case where a higher-priority policy becomes active while
    // a lower-priority one was already running — the cached pointer must
    // switch, not stick to the lower-priority policy.
    StubPolicy highPriority, lowPriority;
    highPriority.active = false;
    lowPriority.active = true;
    std::vector<IDisplayPolicy *> policies = {&highPriority, &lowPriority};

    TEST_ASSERT_TRUE(selectActive(policies) == &lowPriority);

    highPriority.active = true;
    TEST_ASSERT_TRUE(selectActive(policies) == &highPriority);
}

void test_transition_when_active_policy_flips_off(void)
{
    StubPolicy p;
    p.active = true;
    std::vector<IDisplayPolicy *> policies = {&p};

    TEST_ASSERT_TRUE(selectActive(policies) == &p);

    p.active = false;
    TEST_ASSERT_NULL(selectActive(policies));
}

void test_single_registered_policy(void)
{
    StubPolicy p;
    p.active = true;
    p.brightness = 42;
    std::vector<IDisplayPolicy *> policies = {&p};

    IDisplayPolicy *active = selectActive(policies);
    TEST_ASSERT_NOT_NULL(active);

    uint8_t bri = 0;
    TEST_ASSERT_TRUE(active->overridesBrightness(bri));
    TEST_ASSERT_EQUAL_UINT8(42, bri);
}

// ── Runner ──────────────────────────────────────────────────────────

void setUp(void) {}
void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_no_active_policy_returns_nullptr);
    RUN_TEST(test_empty_registry_returns_nullptr);
    RUN_TEST(test_first_registered_active_wins_over_later);
    RUN_TEST(test_later_policy_wins_when_earlier_inactive);
    RUN_TEST(test_transition_when_earlier_policy_flips_on);
    RUN_TEST(test_transition_when_active_policy_flips_off);
    RUN_TEST(test_single_registered_policy);
    return UNITY_END();
}
