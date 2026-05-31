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
    else if (sender == rebootBtn)
    {
        delay(500);
        ESP.restart();
    }
    else if (sender == playSoundBtn)
    {
        mqttSound_->playRTTTLString("beep:d=4,o=5,b=100:16e6");
    }
}

// ── Alarm button callback ───────────────────────────────────────────

/// Snooze or dismiss the ringing alarm from Home Assistant.
/// @param sender The HAButton entity that was pressed.
void onAlarmButtonCommand(HAButton *sender)
{
    if (sender == alarmSnoozeBtn)
    {
        const Alarm *a = AlarmManager.getRingingAlarm();
        AlarmManager.snooze(a ? a->snoozeMinutes : 5);
    }
    else if (sender == alarmDismissBtn)
    {
        AlarmManager.dismiss();
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

/// Handle brightness mode, transition effect, or background effect select from HA.
/// For BriMode: index 0 = Manual, index 1 = Auto brightness.
/// For transEffect: index maps to the transition effect enum.
/// For bgEffect: index 0 = None, indices 1-20 = effect index + 1.
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
            dmControl_->setBrightness(brightnessConfig.brightness);
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
    else if (sender == bgEffect)
    {
        displayConfig.backgroundEffect = index;
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

// ── Night mode callbacks ────────────────────────────────────────────

/// Handle night mode switch commands from HA.
/// Routes to the appropriate config field based on which switch was pressed.
/// @param state New on/off state.
/// @param sender The HASwitch entity that changed.
void onNightSwitchCommand(bool state, HASwitch *sender)
{
    if (sender == nightModeSwitch)
    {
        appConfig.nightMode = state;
    }
    else if (sender == nightBlockSwitch)
    {
        appConfig.nightBlockTransition = state;
    }
    dmControl_->markPolicyConfigDirty();
    saveSettings();
    sender->setState(state);
}

/// Handle night brightness number input from HA.
/// Updates appConfig.nightBrightness and notifies DisplayManager.
/// @param number Numeric value from HA (may be unset on reset).
/// @param sender The HANumber entity that changed.
void onNightNumberCommand(HANumeric number, HANumber *sender)
{
    if (!number.isSet())
    {
        return;
    }
    appConfig.nightBrightness = static_cast<uint8_t>(number.toUInt8());
    dmControl_->markPolicyConfigDirty();
    saveSettings();
    sender->setState(number);
}

/// Handle night color RGB changes from HA.
/// Updates appConfig.nightColor and notifies DisplayManager.
/// @param color RGB color struct from ArduinoHA.
/// @param sender The HALight entity whose color changed.
void onNightColorCommand(HALight::RGBColor color, HALight *sender)
{
    appConfig.nightColor = (color.red << 16) | (color.green << 8) | color.blue;
    dmControl_->markPolicyConfigDirty();
    saveSettings();
    sender->setRGBColor(color);
}

// ── Audio callbacks ─────────────────────────────────────────────────

/// Handle sound enabled switch from HA.
/// @param state New on/off state.
/// @param sender The HASwitch entity that changed.
void onSoundSwitchCommand(bool state, HASwitch *sender)
{
    audioConfig.soundActive = state;
    saveSettings();
    sender->setState(state);
}

/// Handle sound volume number input from HA.
/// @param number Numeric value from HA (may be unset on reset).
/// @param sender The HANumber entity that changed.
void onSoundVolumeCommand(HANumeric number, HANumber *sender)
{
    if (!number.isSet())
        return;
    audioConfig.soundVolume = static_cast<uint8_t>(number.toUInt8());
    mqttPeriphery_->setVolume(audioConfig.soundVolume);
    saveSettings();
    sender->setState(number);
}

// ── App visibility callbacks ────────────────────────────────────────

/// Handle app visibility switch commands from HA.
/// Routes to the appropriate appConfig field based on which switch was pressed.
/// @param state New on/off state.
/// @param sender The HASwitch entity that changed.
void onAppVisibilitySwitchCommand(bool state, HASwitch *sender)
{
    if (sender == showTimeSwitch)
    {
        appConfig.showTime = state;
    }
    else if (sender == showDateSwitch)
    {
        appConfig.showDate = state;
    }
    else if (sender == showTempSwitch)
    {
        appConfig.showTemp = state;
    }
    else if (sender == showHumSwitch)
    {
        appConfig.showHum = state;
    }
    else if (sender == showBatSwitch)
    {
        appConfig.showBat = state;
    }
    dmNav_->loadNativeApps();
    saveSettings();
    sender->setState(state);
}

// ── Display timing callbacks ────────────────────────────────────────

/// Handle display timing number inputs from HA.
/// Routes to the appropriate appConfig field based on which number was changed.
/// @param number Numeric value from HA (may be unset on reset).
/// @param sender The HANumber entity that changed.
void onDisplayTimingCommand(HANumeric number, HANumber *sender)
{
    if (!number.isSet())
        return;

    if (sender == timePerAppNum)
    {
        appConfig.timePerApp = number.toUInt16();
    }
    else if (sender == scrollSpeedNum)
    {
        appConfig.scrollSpeed = static_cast<uint8_t>(number.toUInt8());
    }
    else if (sender == timeDurationNum)
    {
        appConfig.timeDuration = number.toUInt16();
    }
    else if (sender == dateDurationNum)
    {
        appConfig.dateDuration = number.toUInt16();
    }
    saveSettings();
    sender->setState(number);
}

// ── Native app color callbacks ──────────────────────────────────────

/// Handle native app color RGB changes from HA.
/// Routes to the appropriate colorConfig field based on which light was changed.
/// @param color RGB color struct from ArduinoHA.
/// @param sender The HALight entity whose color changed.
void onNativeAppColorCommand(HALight::RGBColor color, HALight *sender)
{
    uint32_t packed = (color.red << 16) | (color.green << 8) | color.blue;

    if (sender == timeColorLight)
    {
        colorConfig.timeColor = packed;
    }
    else if (sender == dateColorLight)
    {
        colorConfig.dateColor = packed;
    }
    else if (sender == tempColorLight)
    {
        colorConfig.tempColor = packed;
    }
    else if (sender == humColorLight)
    {
        colorConfig.humColor = packed;
    }
    else if (sender == batColorLight)
    {
        colorConfig.batColor = packed;
    }
    saveSettings();
    sender->setRGBColor(color);
}

// ── Weather app visibility callbacks ────────────────────────────────

/// Handle weather app visibility switch commands from HA.
/// Routes to the appropriate weatherConfig field based on which switch was pressed.
/// @param state New on/off state.
/// @param sender The HASwitch entity that changed.
void onWeatherVisibilitySwitchCommand(bool state, HASwitch *sender)
{
    if (sender == showOutTempSwitch)
    {
        weatherConfig.showOutdoorTemp = state;
    }
    else if (sender == showOutHumSwitch)
    {
        weatherConfig.showOutdoorHumidity = state;
    }
    else if (sender == showPressureSwitch)
    {
        weatherConfig.showPressure = state;
    }
    else if (sender == showAqiSwitch)
    {
        weatherConfig.showAirQuality = state;
    }
    else if (sender == showUvSwitch)
    {
        weatherConfig.showUV = state;
    }
    dmNav_->loadNativeApps();
    saveSettings();
    sender->setState(state);
}
