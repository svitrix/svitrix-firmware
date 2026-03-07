/**
 * @file MQTTManager_Callbacks.cpp
 * @brief Home Assistant entity callback handlers.
 *
 * Contains all 7 ArduinoHA callback functions that respond to
 * commands from Home Assistant (button presses, switch toggles,
 * select changes, color/brightness/state changes).
 */
#include "MQTTManager_internal.h"

// ── Button callback ─────────────────────────────────────────────────

/// Dispatch HA button presses to the appropriate action.
/// Maps each button entity to its action: dismiss notification,
/// navigate apps (next/prev), or trigger firmware update.
/// @param sender The HAButton entity that was pressed.
void onButtonCommand(HAButton *sender)
{
    if (sender == dismiss)
    {
        dmNotify_->dismissNotify();
    }
    else if (sender == nextApp)
    {
        dmNav_->nextApp();
    }
    else if (sender == prevApp)
    {
        dmNav_->previousApp();
    }
    else if (sender == doUpdate)
    {
        if (mqttUpdater_->checkUpdate(true))
        {
            mqttUpdater_->updateFirmware();
        }
    }
}

// ── Switch callback ─────────────────────────────────────────────────

/// Handle auto-transition toggle from HA.
/// Updates config, applies to DisplayManager, persists, and confirms state.
/// @param state true = auto-transition enabled, false = disabled.
/// @param sender The HASwitch entity that was toggled.
void onSwitchCommand(bool state, HASwitch *sender)
{
    appConfig.autoTransition = state;
    dmControl_->setAutoTransition(state);
    saveSettings();
    sender->setState(state);
}

// ── Select callback ─────────────────────────────────────────────────

/// Handle brightness mode or transition effect select from HA.
/// For BriMode: index 0 = Manual, index 1 = Auto brightness.
/// For transEffect: index maps to the transition effect enum.
/// @param index Zero-based index of the selected option.
/// @param sender The HASelect entity that changed.
void onSelectCommand(int8_t index, HASelect *sender)
{
    if (sender == BriMode)
    {
        switch (index)
        {
        case 0:
            brightnessConfig.autoBrightness = false;
            Matrix->setBrightness(brightnessConfig.brightness, true);
            break;
        case 1:
            brightnessConfig.autoBrightness = true;
            break;
        }
    }
    else if (sender == transEffect)
    {
        appConfig.transEffect = index;
    }
    saveSettings();
    sender->setState(index);
}

// ── RGB color callback ──────────────────────────────────────────────

/// Handle RGB color changes for Matrix and indicator lights from HA.
/// For Matrix: updates global textColor, applies to custom apps, and persists.
/// For indicators: forwards the packed RGB color to DisplayManager.
/// @param color RGB color struct from ArduinoHA (red, green, blue fields).
/// @param sender The HALight entity whose color changed.
void onRGBColorCommand(HALight::RGBColor color, HALight *sender)
{
    if (sender == Matrix)
    {
        colorConfig.textColor = (color.red << 16) | (color.green << 8) | color.blue;
        dmNav_->setCustomAppColors(colorConfig.textColor);
        saveSettings();
    }
    else if (sender == Indikator1)
    {
        dmNotify_->setIndicator1Color((color.red << 16) | (color.green << 8) | color.blue);
    }
    else if (sender == Indikator2)
    {
        dmNotify_->setIndicator2Color((color.red << 16) | (color.green << 8) | color.blue);
    }
    else if (sender == Indikator3)
    {
        dmNotify_->setIndicator3Color((color.red << 16) | (color.green << 8) | color.blue);
    }
    sender->setRGBColor(color);
}

// ── State callback ──────────────────────────────────────────────────

/// Handle on/off state changes for Matrix and indicator lights from HA.
/// For Matrix: controls display power. For indicators: toggles visibility.
/// @param state true = on, false = off.
/// @param sender The HALight entity whose state changed.
void onStateCommand(bool state, HALight *sender)
{
    if (sender == Matrix)
    {
        dmControl_->setPower(state);
    }
    else if (sender == Indikator1)
    {
        dmNotify_->setIndicator1State(state);
    }
    else if (sender == Indikator2)
    {
        dmNotify_->setIndicator2State(state);
    }
    else if (sender == Indikator3)
    {
        dmNotify_->setIndicator3State(state);
    }
    sender->setState(state);
}

// ── Brightness callback ─────────────────────────────────────────────

/// Handle brightness slider changes from HA.
/// Ignored when auto-brightness is active; otherwise updates config,
/// persists settings, and applies to DisplayManager.
/// @param brightness New brightness level (0–255).
/// @param sender The HALight entity whose brightness changed.
void onBrightnessCommand(uint8_t brightness, HALight *sender)
{
    sender->setBrightness(brightness);
    if (brightnessConfig.autoBrightness)
        return;
    brightnessConfig.brightness = brightness;
    saveSettings();
    dmControl_->setBrightness(brightness);
}

// ── Number callback ─────────────────────────────────────────────────

/// Handle scroll speed number input from HA.
/// When the value is unset (reset command), no action is taken.
/// Otherwise updates appConfig.scrollSpeed and persists.
/// @param number Numeric value from HA (may be unset on reset).
/// @param sender The HANumber entity that changed.
void onNumberCommand(HANumeric number, HANumber *sender)
{
    if (!number.isSet())
    {
        // reset command from Home Assistant
    }
    else
    {
        appConfig.scrollSpeed = number.toInt8();
        saveSettings();
    }

    sender->setState(number);
}
