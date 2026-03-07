/**
 * @file NotificationManager.h
 * @brief Notification and indicator engine extracted from DisplayManager_ (Phase 11.2).
 *
 * NotificationManager_ owns notification parsing/display/dismissal and
 * LED indicator control. Implements IDisplayNotifier for consumer injection.
 *
 * Dependencies injected via setDependencies():
 *   INotifier*          — MQTT publishing for indicator state changes
 *   IPeripheryProvider* — sound stop on notification dismiss
 */
#pragma once

#include <Arduino.h>
#include "IDisplayNotifier.h"

class INotifier;
class IPeripheryProvider;

class NotificationManager_ : public IDisplayNotifier
{
  public:
    /// Inject dependencies from DisplayManager_ (called once during wiring).
    void setDependencies(INotifier *n, IPeripheryProvider *p);

    // ── IDisplayNotifier interface (9 methods) ──────────────────────
    bool generateNotification(uint8_t source, const char *json) override;
    void dismissNotify() override;
    bool indicatorParser(uint8_t indicator, const char *json) override;
    void setIndicator1Color(uint32_t color) override;
    void setIndicator1State(bool state) override;
    void setIndicator2Color(uint32_t color) override;
    void setIndicator2State(bool state) override;
    void setIndicator3Color(uint32_t color) override;
    void setIndicator3State(bool state) override;

  private:
    INotifier *notifier_ = nullptr;
    IPeripheryProvider *periphery_ = nullptr;
};
