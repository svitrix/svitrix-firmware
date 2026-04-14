#pragma once

#include <cstdint>

/**
 * @file IDisplayPolicy.h
 * @brief Display override policy — a time-bounded or event-driven rule that
 *        can override brightness, text color, and auto-transition behavior.
 *
 * Policies are a composition hook: DisplayManager holds a list of them and
 * consults the first active one on each tick. Adding a new mode (movie,
 * away, sleep timer, etc.) means adding a new implementor, not editing
 * DisplayManager.
 *
 * Resolution rule: the FIRST registered policy whose isActive() returns
 * true wins. Order of registration in main.cpp defines priority.
 *
 * Implementors must be pure (no side effects from isActive / overrides*);
 * DisplayManager may call them multiple times per frame.
 *
 * Callers must check isActive() before calling the overrides* methods;
 * their behavior is undefined when the policy is inactive. This keeps
 * hot-path implementations free of redundant isActive() recomputation.
 */
class IDisplayPolicy
{
public:
    virtual ~IDisplayPolicy() = default;

    /// @return true if this policy wants its overrides applied right now.
    virtual bool isActive() const = 0;

    /// @param out Brightness value to apply while this policy is active.
    /// @return true if this policy overrides brightness (out is populated).
    virtual bool overridesBrightness(uint8_t& out) const = 0;

    /// @param out Packed 0xRRGGBB color to apply while this policy is active.
    /// @return true if this policy overrides text color (out is populated).
    virtual bool overridesTextColor(uint32_t& out) const = 0;

    /// @return true if auto-transition must be disabled while active.
    virtual bool blocksAutoTransition() const = 0;
};
