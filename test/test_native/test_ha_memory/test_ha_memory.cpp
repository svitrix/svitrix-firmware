/**
 * @file test_ha_memory.cpp
 * @brief Tests for the destroy-and-reset pattern used by destroyHAEntities()
 *        and HAMqtt::resetDevicesCount().
 *
 * The actual HA library can't compile in native_test (needs WiFi, PubSubClient,
 * IPAddress, etc.), so we verify the contract with lightweight mocks that
 * replicate the exact data structures and logic.
 */

#include <unity.h>
#include "HADiscovery.h"
#include <cstdint>

// ── Destructor-tracking mock ────────────────────────────────────────

static int g_dtorCalls = 0;

struct MockEntity
{
    int id;
    MockEntity(int id_) : id(id_) {}
    ~MockEntity() { g_dtorCalls++; }
};

// ── Device registry mock (mirrors HAMqtt's addDeviceType/resetDevicesCount) ─

struct DeviceRegistry
{
    uint8_t count;
    uint8_t maxCount;
    MockEntity **devices;

    DeviceRegistry(uint8_t maxDevices)
        : count(0), maxCount(maxDevices),
          devices(new MockEntity *[maxDevices])
    {
    }
    ~DeviceRegistry() { delete[] devices; }

    void addDevice(MockEntity *dev)
    {
        if (count + 1 >= maxCount)
            return;
        devices[count++] = dev;
    }

    void resetCount() { count = 0; }
};

// ── 25 global pointers (mirrors MQTTManager_internal.h extern ptrs) ─

static MockEntity *e[25];

static void allocateAll(DeviceRegistry &reg)
{
    for (int i = 0; i < 25; i++)
    {
        e[i] = new MockEntity(i);
        reg.addDevice(e[i]);
    }
}

/// Exact pattern from destroyHAEntities(): delete + nullptr + resetCount
static void destroyAll(DeviceRegistry &reg)
{
    for (int i = 0; i < 25; i++)
    {
        delete e[i];
        e[i] = nullptr;
    }
    reg.resetCount();
}

// ── Tests: resetDevicesCount ────────────────────────────────────────

void test_resetDevicesCount_sets_count_to_zero(void)
{
    DeviceRegistry reg(30);
    MockEntity a(1), b(2), c(3);
    reg.addDevice(&a);
    reg.addDevice(&b);
    reg.addDevice(&c);
    TEST_ASSERT_EQUAL(3, reg.count);

    reg.resetCount();
    TEST_ASSERT_EQUAL(0, reg.count);
}

void test_resetDevicesCount_allows_reregistration(void)
{
    DeviceRegistry reg(30);
    MockEntity a(1), b(2);
    reg.addDevice(&a);
    reg.addDevice(&b);
    TEST_ASSERT_EQUAL(2, reg.count);

    reg.resetCount();
    MockEntity c(3);
    reg.addDevice(&c);
    TEST_ASSERT_EQUAL(1, reg.count);
    TEST_ASSERT_EQUAL(3, reg.devices[0]->id);
}

void test_addDevice_overflow_guard(void)
{
    DeviceRegistry reg(3); // max 3, guard at count+1 >= max
    MockEntity a(1), b(2), c(3);
    reg.addDevice(&a);
    reg.addDevice(&b);
    reg.addDevice(&c); // should be ignored: 2+1 >= 3
    TEST_ASSERT_EQUAL(2, reg.count);
}

void test_addDevice_after_reset_respects_max(void)
{
    DeviceRegistry reg(3);
    MockEntity a(1), b(2);
    reg.addDevice(&a);
    reg.addDevice(&b);
    TEST_ASSERT_EQUAL(2, reg.count);

    reg.resetCount();
    MockEntity c(3), d(4), e(5);
    reg.addDevice(&c);
    reg.addDevice(&d);
    reg.addDevice(&e); // overflow again
    TEST_ASSERT_EQUAL(2, reg.count);
}

// ── Tests: destroy pattern ──────────────────────────────────────────

void test_destroy_nulls_all_pointers(void)
{
    DeviceRegistry reg(30);
    g_dtorCalls = 0;
    allocateAll(reg);

    destroyAll(reg);

    for (int i = 0; i < 25; i++)
    {
        TEST_ASSERT_NULL(e[i]);
    }
}

void test_destroy_frees_all_objects(void)
{
    DeviceRegistry reg(30);
    g_dtorCalls = 0;
    allocateAll(reg);

    destroyAll(reg);

    TEST_ASSERT_EQUAL(25, g_dtorCalls);
}

void test_destroy_resets_registry_count(void)
{
    DeviceRegistry reg(30);
    g_dtorCalls = 0;
    allocateAll(reg);
    TEST_ASSERT_EQUAL(25, reg.count);

    destroyAll(reg);

    TEST_ASSERT_EQUAL(0, reg.count);
}

void test_double_destroy_is_safe(void)
{
    DeviceRegistry reg(30);
    g_dtorCalls = 0;
    allocateAll(reg);

    destroyAll(reg);
    TEST_ASSERT_EQUAL(25, g_dtorCalls);

    // Second destroy must be a no-op (delete nullptr is safe in C++)
    destroyAll(reg);
    TEST_ASSERT_EQUAL(25, g_dtorCalls); // no additional destructor calls
}

void test_destroy_then_reallocate(void)
{
    DeviceRegistry reg(30);
    g_dtorCalls = 0;
    allocateAll(reg);

    destroyAll(reg);
    TEST_ASSERT_EQUAL(25, g_dtorCalls);
    TEST_ASSERT_EQUAL(0, reg.count);

    // Re-allocate — simulates calling setup() again
    allocateAll(reg);
    TEST_ASSERT_EQUAL(25, reg.count);
    for (int i = 0; i < 25; i++)
    {
        TEST_ASSERT_NOT_NULL(e[i]);
    }

    // Cleanup
    destroyAll(reg);
    TEST_ASSERT_EQUAL(50, g_dtorCalls); // 25 + 25
}

void test_destroy_nullptr_pointers_is_safe(void)
{
    DeviceRegistry reg(30);
    // All pointers start as nullptr (never allocated)
    for (int i = 0; i < 25; i++)
        e[i] = nullptr;

    g_dtorCalls = 0;
    destroyAll(reg);

    TEST_ASSERT_EQUAL(0, g_dtorCalls);
    for (int i = 0; i < 25; i++)
    {
        TEST_ASSERT_NULL(e[i]);
    }
}

// ── Tests: entity count consistency ─────────────────────────────────

void test_entity_count_matches_destroy_count(void)
{
    // destroyHAEntities() deletes exactly 25 pointers.
    // getTotalEntityCount(true) must return the same number.
    TEST_ASSERT_EQUAL(25, getTotalEntityCount(true));
}

// ── Runner ──────────────────────────────────────────────────────────

void setUp(void) {}
void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();

    // resetDevicesCount pattern
    RUN_TEST(test_resetDevicesCount_sets_count_to_zero);
    RUN_TEST(test_resetDevicesCount_allows_reregistration);
    RUN_TEST(test_addDevice_overflow_guard);
    RUN_TEST(test_addDevice_after_reset_respects_max);

    // destroy pattern
    RUN_TEST(test_destroy_nulls_all_pointers);
    RUN_TEST(test_destroy_frees_all_objects);
    RUN_TEST(test_destroy_resets_registry_count);
    RUN_TEST(test_double_destroy_is_safe);
    RUN_TEST(test_destroy_then_reallocate);
    RUN_TEST(test_destroy_nullptr_pointers_is_safe);

    // consistency
    RUN_TEST(test_entity_count_matches_destroy_count);

    return UNITY_END();
}
