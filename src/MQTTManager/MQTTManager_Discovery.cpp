/**
 * @file MQTTManager_Discovery.cpp
 * @brief Home Assistant auto-discovery entity creation.
 *
 * Contains MQTTManager_::setup() which initializes all ArduinoHA
 * entities using descriptors from HADiscovery. Entity metadata
 * (IDs, names, icons, units) comes from HADiscovery; ArduinoHA
 * object creation and callback registration happen here.
 */
#include "MQTTManager_internal.h"

// ── Cleanup ─────────────────────────────────────────────────────────

/// Delete all HA entity objects and reset their pointers to nullptr.
/// Also resets HAMqtt's internal device type counter so entities can
/// be re-registered on the next setup() call without overflow.
static void destroyHAEntities()
{
    delete Matrix;
    Matrix = nullptr;
    delete Indikator1;
    Indikator1 = nullptr;
    delete Indikator2;
    Indikator2 = nullptr;
    delete Indikator3;
    Indikator3 = nullptr;
    delete BriMode;
    BriMode = nullptr;
    delete transEffect;
    transEffect = nullptr;
    delete bgEffect;
    bgEffect = nullptr;
    delete dismiss;
    dismiss = nullptr;
    delete nextApp;
    nextApp = nullptr;
    delete prevApp;
    prevApp = nullptr;
    delete doUpdate;
    doUpdate = nullptr;
    delete rebootBtn;
    rebootBtn = nullptr;
    delete playSoundBtn;
    playSoundBtn = nullptr;
    delete soundEnabled;
    soundEnabled = nullptr;
    delete soundVolume;
    soundVolume = nullptr;
    delete transition;
    transition = nullptr;
    delete battery;
    battery = nullptr;
    delete temperature;
    temperature = nullptr;
    delete humidity;
    humidity = nullptr;
    delete illuminance;
    illuminance = nullptr;
    delete uptime;
    uptime = nullptr;
    delete strength;
    strength = nullptr;
    delete version;
    version = nullptr;
    delete ram;
    ram = nullptr;
    delete curApp;
    curApp = nullptr;
    delete myOwnID;
    myOwnID = nullptr;
    delete ipAddr;
    ipAddr = nullptr;
    delete btnleft;
    btnleft = nullptr;
    delete btnmid;
    btnmid = nullptr;
    delete btnright;
    btnright = nullptr;
    delete outdoorTemp;
    outdoorTemp = nullptr;
    delete outdoorHum;
    outdoorHum = nullptr;
    delete pressure;
    pressure = nullptr;
    delete aqi;
    aqi = nullptr;
    delete weatherCond;
    weatherCond = nullptr;
    delete uvIndex;
    uvIndex = nullptr;

    // Night mode
    delete nightModeSwitch;
    nightModeSwitch = nullptr;
    delete nightBrightnessNum;
    nightBrightnessNum = nullptr;
    delete nightColorLight;
    nightColorLight = nullptr;
    delete nightBlockSwitch;
    nightBlockSwitch = nullptr;

    // App visibility
    delete showTimeSwitch;
    showTimeSwitch = nullptr;
    delete showDateSwitch;
    showDateSwitch = nullptr;
    delete showTempSwitch;
    showTempSwitch = nullptr;
    delete showHumSwitch;
    showHumSwitch = nullptr;
    delete showBatSwitch;
    showBatSwitch = nullptr;

    // Display timing
    delete timePerAppNum;
    timePerAppNum = nullptr;
    delete scrollSpeedNum;
    scrollSpeedNum = nullptr;
    delete timeDurationNum;
    timeDurationNum = nullptr;
    delete dateDurationNum;
    dateDurationNum = nullptr;

    // Native app colors
    delete timeColorLight;
    timeColorLight = nullptr;
    delete dateColorLight;
    dateColorLight = nullptr;
    delete tempColorLight;
    tempColorLight = nullptr;
    delete humColorLight;
    humColorLight = nullptr;
    delete batColorLight;
    batColorLight = nullptr;

    // Weather app visibility
    delete showOutTempSwitch;
    showOutTempSwitch = nullptr;
    delete showOutHumSwitch;
    showOutHumSwitch = nullptr;
    delete showPressureSwitch;
    showPressureSwitch = nullptr;
    delete showAqiSwitch;
    showAqiSwitch = nullptr;
    delete showUvSwitch;
    showUvSwitch = nullptr;

    // Alarm entities
    delete alarmRinging;
    alarmRinging = nullptr;
    delete alarmSnoozeBtn;
    alarmSnoozeBtn = nullptr;
    delete alarmDismissBtn;
    alarmDismissBtn = nullptr;
    delete nextAlarmSensor;
    nextAlarmSensor = nullptr;

    mqtt.resetDevicesCount();
}

// ── Helper ──────────────────────────────────────────────────────────

/// Apply metadata from a descriptor to an HASensor entity.
/// Sets name unconditionally; icon, deviceClass, and unit are only
/// applied when the corresponding descriptor field is non-null.
/// @param sensor Pointer to the HASensor to configure.
/// @param desc   Descriptor containing the metadata to apply.
static void applySensorDescriptor(HASensor *sensor, const HAEntityDescriptor& desc)
{
    sensor->setName(desc.name);
    if (desc.icon)
        sensor->setIcon(desc.icon);
    if (desc.deviceClass)
        sensor->setDeviceClass(desc.deviceClass);
    if (desc.unit)
        sensor->setUnitOfMeasurement(desc.unit);
}

// ── HA Discovery setup ──────────────────────────────────────────────

/// Initialize all ArduinoHA entities and connect to the MQTT broker.
///
/// When HA discovery is enabled (haConfig.discovery == true):
///   1. Configures the HADevice with MAC, hostname, version, and IP.
///   2. Creates all HA entities (lights, selects, buttons, switch,
///      sensors, binary sensors) using descriptors from HADiscovery.
///   3. Registers ArduinoHA callback handlers for interactive entities.
///
/// When HA discovery is disabled, calls mqtt.disableHA() instead.
/// Always calls connect() at the end to initiate the broker connection.
void MQTTManager_::setup()
{
    if (haConfig.discovery)
    {
        destroyHAEntities();

        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Starting Homeassistant discovery"));
        mqtt.setDiscoveryPrefix(haConfig.prefix.c_str());
        mqtt.setDataPrefix(mqttConfig.prefix.c_str());
        uint8_t mac[6];
        WiFi.macAddress(mac);
        char macStr[7];
        snprintf(macStr, 7, "%02x%02x%02x", mac[3], mac[4], mac[5]);
        device.setUniqueId(mac, sizeof(mac));
        device.setName(systemConfig.hostname.c_str());
        device.setSoftwareVersion(VERSION);
        device.setManufacturer(getDeviceManufacturer());
        device.setModel(getDeviceModel());
        device.setAvailability(true);
        device.enableSharedAvailability();
        device.enableLastWill();

        IPAddress ip = WiFi.localIP();
        static char configurationUrl[32];
        uint8_t ipBytes[4] = {(uint8_t)ip[0], (uint8_t)ip[1], (uint8_t)ip[2], (uint8_t)ip[3]};
        buildConfigUrl(ipBytes, configurationUrl, sizeof(configurationUrl));
        device.setConfigurationUrl(configurationUrl);

        // Matrix light
        auto matDesc = getMatrixLightDescriptor();
        buildEntityId(matDesc.idTemplate, macStr, matID, sizeof(matID));
        Matrix = new HALight(matID, HALight::BrightnessFeature | HALight::RGBFeature);
        Matrix->setIcon(matDesc.icon);
        Matrix->setName(matDesc.name);
        Matrix->onStateCommand(onStateCommand);
        Matrix->onBrightnessCommand(onBrightnessCommand);
        Matrix->onRGBColorCommand(onRGBColorCommand);
        Matrix->setCurrentState(true);
        Matrix->setBRIGHTNESS(brightnessConfig.brightness);

        HALight::RGBColor color;
        color.isSet = true;
        color.red = (colorConfig.textColor >> 16) & 0xFF;
        color.green = (colorConfig.textColor >> 8) & 0xFF;
        color.blue = colorConfig.textColor & 0xFF;
        Matrix->setCurrentRGBColor(color);
        Matrix->setState(displayConfig.matrixOff, true);

        // Indicator lights
        size_t indCount;
        const auto *indDescs = getIndicatorLightDescriptors(indCount);

        buildEntityId(indDescs[0].idTemplate, macStr, ind1ID, sizeof(ind1ID));
        Indikator1 = new HALight(ind1ID, HALight::RGBFeature);
        Indikator1->setIcon(indDescs[0].icon);
        Indikator1->setName(indDescs[0].name);
        Indikator1->onStateCommand(onStateCommand);
        Indikator1->onRGBColorCommand(onRGBColorCommand);

        buildEntityId(indDescs[1].idTemplate, macStr, ind2ID, sizeof(ind2ID));
        Indikator2 = new HALight(ind2ID, HALight::RGBFeature);
        Indikator2->setIcon(indDescs[1].icon);
        Indikator2->setName(indDescs[1].name);
        Indikator2->onStateCommand(onStateCommand);
        Indikator2->onRGBColorCommand(onRGBColorCommand);

        buildEntityId(indDescs[2].idTemplate, macStr, ind3ID, sizeof(ind3ID));
        Indikator3 = new HALight(ind3ID, HALight::RGBFeature);
        Indikator3->setIcon(indDescs[2].icon);
        Indikator3->setName(indDescs[2].name);
        Indikator3->onStateCommand(onStateCommand);
        Indikator3->onRGBColorCommand(onRGBColorCommand);

        // Selects
        size_t selCount;
        const auto *selDescs = getSelectDescriptors(selCount);

        buildEntityId(selDescs[0].base.idTemplate, macStr, briID, sizeof(briID));
        BriMode = new HASelect(briID);
        BriMode->setOptions(selDescs[0].options);
        BriMode->onCommand(onSelectCommand);
        BriMode->setIcon(selDescs[0].base.icon);
        BriMode->setName(selDescs[0].base.name);
        BriMode->setState(brightnessConfig.autoBrightness, true);

        buildEntityId(selDescs[1].base.idTemplate, macStr, effectID, sizeof(effectID));
        transEffect = new HASelect(effectID);
        transEffect->setOptions(selDescs[1].options);
        transEffect->onCommand(onSelectCommand);
        transEffect->setIcon(selDescs[1].base.icon);
        transEffect->setName(selDescs[1].base.name);
        transEffect->setState(appConfig.transEffect, true);

        // Background effect select
        auto bgEffectDesc = getBackgroundEffectDescriptor();
        buildEntityId(bgEffectDesc.base.idTemplate, macStr, bgEffectID, sizeof(bgEffectID));
        bgEffect = new HASelect(bgEffectID);
        bgEffect->setOptions(bgEffectDesc.options);
        bgEffect->onCommand(onSelectCommand);
        bgEffect->setIcon(bgEffectDesc.base.icon);
        bgEffect->setName(bgEffectDesc.base.name);
        bgEffect->setState(displayConfig.backgroundEffect, true);

        // Buttons
        size_t btnCount;
        const auto *btnDescs = getButtonDescriptors(btnCount);

        buildEntityId(btnDescs[0].idTemplate, macStr, btnAID, sizeof(btnAID));
        dismiss = new HAButton(btnAID);
        dismiss->setIcon(btnDescs[0].icon);
        dismiss->setName(btnDescs[0].name);

        buildEntityId(btnDescs[1].idTemplate, macStr, doUpdateID, sizeof(doUpdateID));
        doUpdate = new HAButton(doUpdateID);
        doUpdate->setIcon(btnDescs[1].icon);
        doUpdate->setName(btnDescs[1].name);
        doUpdate->onCommand(onButtonCommand);

        // Transition switch
        auto swDesc = getTransitionSwitchDescriptor();
        buildEntityId(swDesc.idTemplate, macStr, transID, sizeof(transID));
        transition = new HASwitch(transID);
        transition->setIcon(swDesc.icon);
        transition->setName(swDesc.name);
        transition->onCommand(onSwitchCommand);

        // Sensors (battery is at index 10, always last)
        size_t senCount;
        const auto *senDescs = getSensorDescriptors(senCount, true);

        buildEntityId(senDescs[0].idTemplate, macStr, appID, sizeof(appID));
        curApp = new HASensor(appID);
        applySensorDescriptor(curApp, senDescs[0]);

        buildEntityId(senDescs[1].idTemplate, macStr, myID, sizeof(myID));
        myOwnID = new HASensor(myID);
        applySensorDescriptor(myOwnID, senDescs[1]);

        buildEntityId(btnDescs[2].idTemplate, macStr, btnBID, sizeof(btnBID));
        nextApp = new HAButton(btnBID);
        nextApp->setIcon(btnDescs[2].icon);
        nextApp->setName(btnDescs[2].name);

        buildEntityId(btnDescs[3].idTemplate, macStr, btnCID, sizeof(btnCID));
        prevApp = new HAButton(btnCID);
        prevApp->setIcon(btnDescs[3].icon);
        prevApp->setName(btnDescs[3].name);

        buildEntityId(btnDescs[4].idTemplate, macStr, rebootID, sizeof(rebootID));
        rebootBtn = new HAButton(rebootID);
        rebootBtn->setIcon(btnDescs[4].icon);
        rebootBtn->setName(btnDescs[4].name);
        rebootBtn->onCommand(onButtonCommand);

        buildEntityId(btnDescs[5].idTemplate, macStr, playSoundID, sizeof(playSoundID));
        playSoundBtn = new HAButton(playSoundID);
        playSoundBtn->setIcon(btnDescs[5].icon);
        playSoundBtn->setName(btnDescs[5].name);
        playSoundBtn->onCommand(onButtonCommand);

        dismiss->onCommand(onButtonCommand);
        nextApp->onCommand(onButtonCommand);
        prevApp->onCommand(onButtonCommand);

        // Audio controls
        size_t audioCount;
        const auto *audioDescs = getAudioDescriptors(audioCount);

        buildEntityId(audioDescs[0].idTemplate, macStr, soundEnID, sizeof(soundEnID));
        soundEnabled = new HASwitch(soundEnID);
        soundEnabled->setIcon(audioDescs[0].icon);
        soundEnabled->setName(audioDescs[0].name);
        soundEnabled->onCommand(onSoundSwitchCommand);
        soundEnabled->setState(audioConfig.soundActive, true);

        buildEntityId(audioDescs[1].idTemplate, macStr, soundVolID, sizeof(soundVolID));
        soundVolume = new HANumber(soundVolID);
        soundVolume->setIcon(audioDescs[1].icon);
        soundVolume->setName(audioDescs[1].name);
        soundVolume->setMin(0);
        soundVolume->setMax(30);
        soundVolume->setStep(1);
        soundVolume->onCommand(onSoundVolumeCommand);
        soundVolume->setState(audioConfig.soundVolume);

        buildEntityId(senDescs[2].idTemplate, macStr, tempID, sizeof(tempID));
        temperature = new HASensor(tempID);
        applySensorDescriptor(temperature, senDescs[2]);

        buildEntityId(senDescs[3].idTemplate, macStr, humID, sizeof(humID));
        humidity = new HASensor(humID);
        applySensorDescriptor(humidity, senDescs[3]);

#ifdef ULANZI
        buildEntityId(senDescs[10].idTemplate, macStr, batID, sizeof(batID));
        battery = new HASensor(batID);
        applySensorDescriptor(battery, senDescs[10]);
#endif

        buildEntityId(senDescs[4].idTemplate, macStr, luxID, sizeof(luxID));
        illuminance = new HASensor(luxID);
        applySensorDescriptor(illuminance, senDescs[4]);

        buildEntityId(senDescs[5].idTemplate, macStr, verID, sizeof(verID));
        version = new HASensor(verID);
        applySensorDescriptor(version, senDescs[5]);

        buildEntityId(senDescs[6].idTemplate, macStr, sigID, sizeof(sigID));
        strength = new HASensor(sigID);
        applySensorDescriptor(strength, senDescs[6]);

        buildEntityId(senDescs[7].idTemplate, macStr, upID, sizeof(upID));
        uptime = new HASensor(upID);
        applySensorDescriptor(uptime, senDescs[7]);

        // Binary sensors
        size_t binCount;
        const auto *binDescs = getBinarySensorDescriptors(binCount);

        buildEntityId(binDescs[0].idTemplate, macStr, btnLID, sizeof(btnLID));
        btnleft = new HABinarySensor(btnLID);
        btnleft->setName(binDescs[0].name);

        buildEntityId(binDescs[1].idTemplate, macStr, btnMID, sizeof(btnMID));
        btnmid = new HABinarySensor(btnMID);
        btnmid->setName(binDescs[1].name);

        buildEntityId(binDescs[2].idTemplate, macStr, btnRID, sizeof(btnRID));
        btnright = new HABinarySensor(btnRID);
        btnright->setName(binDescs[2].name);

        buildEntityId(senDescs[8].idTemplate, macStr, ramID, sizeof(ramID));
        ram = new HASensor(ramID);
        applySensorDescriptor(ram, senDescs[8]);

        buildEntityId(senDescs[9].idTemplate, macStr, ipAddrID, sizeof(ipAddrID));
        ipAddr = new HASensor(ipAddrID);
        applySensorDescriptor(ipAddr, senDescs[9]);

        // Weather sensors (from WeatherAPI)
        size_t weatherCount;
        const auto *weatherDescs = getWeatherSensorDescriptors(weatherCount);

        buildEntityId(weatherDescs[0].idTemplate, macStr, outTempID, sizeof(outTempID));
        outdoorTemp = new HASensor(outTempID);
        outdoorTemp->setName(weatherDescs[0].name);
        outdoorTemp->setIcon(weatherDescs[0].icon);
        outdoorTemp->setDeviceClass(weatherDescs[0].deviceClass);
        const char *tempUnit = timeConfig.isCelsius ? "\xC2\xB0"
                                                      "C"
                                                    : "\xC2\xB0"
                                                      "F";
        outdoorTemp->setUnitOfMeasurement(tempUnit);

        buildEntityId(weatherDescs[1].idTemplate, macStr, outHumID, sizeof(outHumID));
        outdoorHum = new HASensor(outHumID);
        applySensorDescriptor(outdoorHum, weatherDescs[1]);

        buildEntityId(weatherDescs[2].idTemplate, macStr, pressID, sizeof(pressID));
        pressure = new HASensor(pressID);
        applySensorDescriptor(pressure, weatherDescs[2]);

        buildEntityId(weatherDescs[3].idTemplate, macStr, aqiID, sizeof(aqiID));
        aqi = new HASensor(aqiID);
        aqi->setName(weatherDescs[3].name);
        aqi->setIcon(weatherDescs[3].icon);
        aqi->setDeviceClass(weatherDescs[3].deviceClass);

        buildEntityId(weatherDescs[4].idTemplate, macStr, weatherCondID, sizeof(weatherCondID));
        weatherCond = new HASensor(weatherCondID);
        weatherCond->setName(weatherDescs[4].name);
        weatherCond->setIcon(weatherDescs[4].icon);

        buildEntityId(weatherDescs[5].idTemplate, macStr, uvID, sizeof(uvID));
        uvIndex = new HASensor(uvID);
        uvIndex->setName(weatherDescs[5].name);
        uvIndex->setIcon(weatherDescs[5].icon);

        // Night mode controls
        size_t nightCount;
        const auto *nightDescs = getNightModeDescriptors(nightCount);

        // Night mode switch (on/off)
        buildEntityId(nightDescs[0].idTemplate, macStr, nightModeID, sizeof(nightModeID));
        nightModeSwitch = new HASwitch(nightModeID);
        nightModeSwitch->setIcon(nightDescs[0].icon);
        nightModeSwitch->setName(nightDescs[0].name);
        nightModeSwitch->onCommand(onNightSwitchCommand);
        nightModeSwitch->setState(appConfig.nightMode, true);

        // Night brightness number (1-50)
        buildEntityId(nightDescs[1].idTemplate, macStr, nightBriID, sizeof(nightBriID));
        nightBrightnessNum = new HANumber(nightBriID);
        nightBrightnessNum->setIcon(nightDescs[1].icon);
        nightBrightnessNum->setName(nightDescs[1].name);
        nightBrightnessNum->setMin(1);
        nightBrightnessNum->setMax(50);
        nightBrightnessNum->setStep(1);
        nightBrightnessNum->onCommand(onNightNumberCommand);
        nightBrightnessNum->setState(appConfig.nightBrightness);

        // Night color light (RGB only)
        buildEntityId(nightDescs[2].idTemplate, macStr, nightColID, sizeof(nightColID));
        nightColorLight = new HALight(nightColID, HALight::RGBFeature);
        nightColorLight->setIcon(nightDescs[2].icon);
        nightColorLight->setName(nightDescs[2].name);
        nightColorLight->onRGBColorCommand(onNightColorCommand);
        HALight::RGBColor nightCol;
        nightCol.isSet = true;
        nightCol.red = (appConfig.nightColor >> 16) & 0xFF;
        nightCol.green = (appConfig.nightColor >> 8) & 0xFF;
        nightCol.blue = appConfig.nightColor & 0xFF;
        nightColorLight->setCurrentRGBColor(nightCol);
        nightColorLight->setCurrentState(true);

        // Night block transition switch
        buildEntityId(nightDescs[3].idTemplate, macStr, nightBlockID, sizeof(nightBlockID));
        nightBlockSwitch = new HASwitch(nightBlockID);
        nightBlockSwitch->setIcon(nightDescs[3].icon);
        nightBlockSwitch->setName(nightDescs[3].name);
        nightBlockSwitch->onCommand(onNightSwitchCommand);
        nightBlockSwitch->setState(appConfig.nightBlockTransition, true);

        // App visibility switches
        size_t appVisCount;
        const auto *appVisDescs = getAppVisibilityDescriptors(appVisCount);

        buildEntityId(appVisDescs[0].idTemplate, macStr, showTimeID, sizeof(showTimeID));
        showTimeSwitch = new HASwitch(showTimeID);
        showTimeSwitch->setIcon(appVisDescs[0].icon);
        showTimeSwitch->setName(appVisDescs[0].name);
        showTimeSwitch->onCommand(onAppVisibilitySwitchCommand);
        showTimeSwitch->setState(appConfig.showTime, true);

        buildEntityId(appVisDescs[1].idTemplate, macStr, showDateID, sizeof(showDateID));
        showDateSwitch = new HASwitch(showDateID);
        showDateSwitch->setIcon(appVisDescs[1].icon);
        showDateSwitch->setName(appVisDescs[1].name);
        showDateSwitch->onCommand(onAppVisibilitySwitchCommand);
        showDateSwitch->setState(appConfig.showDate, true);

        buildEntityId(appVisDescs[2].idTemplate, macStr, showTempID, sizeof(showTempID));
        showTempSwitch = new HASwitch(showTempID);
        showTempSwitch->setIcon(appVisDescs[2].icon);
        showTempSwitch->setName(appVisDescs[2].name);
        showTempSwitch->onCommand(onAppVisibilitySwitchCommand);
        showTempSwitch->setState(appConfig.showTemp, true);

        buildEntityId(appVisDescs[3].idTemplate, macStr, showHumID, sizeof(showHumID));
        showHumSwitch = new HASwitch(showHumID);
        showHumSwitch->setIcon(appVisDescs[3].icon);
        showHumSwitch->setName(appVisDescs[3].name);
        showHumSwitch->onCommand(onAppVisibilitySwitchCommand);
        showHumSwitch->setState(appConfig.showHum, true);

        buildEntityId(appVisDescs[4].idTemplate, macStr, showBatID, sizeof(showBatID));
        showBatSwitch = new HASwitch(showBatID);
        showBatSwitch->setIcon(appVisDescs[4].icon);
        showBatSwitch->setName(appVisDescs[4].name);
        showBatSwitch->onCommand(onAppVisibilitySwitchCommand);
        showBatSwitch->setState(appConfig.showBat, true);

        // Display timing numbers
        size_t timingCount;
        const auto *timingDescs = getDisplayTimingDescriptors(timingCount);

        // Time per app (1-60 seconds)
        buildEntityId(timingDescs[0].idTemplate, macStr, timePerAppID, sizeof(timePerAppID));
        timePerAppNum = new HANumber(timePerAppID);
        timePerAppNum->setIcon(timingDescs[0].icon);
        timePerAppNum->setName(timingDescs[0].name);
        timePerAppNum->setUnitOfMeasurement(timingDescs[0].unit);
        timePerAppNum->setMin(1);
        timePerAppNum->setMax(60);
        timePerAppNum->setStep(1);
        timePerAppNum->onCommand(onDisplayTimingCommand);
        timePerAppNum->setState(static_cast<float>(appConfig.timePerApp));

        // Scroll speed (20-200 ms)
        buildEntityId(timingDescs[1].idTemplate, macStr, scrollSpeedID, sizeof(scrollSpeedID));
        scrollSpeedNum = new HANumber(scrollSpeedID);
        scrollSpeedNum->setIcon(timingDescs[1].icon);
        scrollSpeedNum->setName(timingDescs[1].name);
        scrollSpeedNum->setUnitOfMeasurement(timingDescs[1].unit);
        scrollSpeedNum->setMin(20);
        scrollSpeedNum->setMax(200);
        scrollSpeedNum->setStep(5);
        scrollSpeedNum->onCommand(onDisplayTimingCommand);
        scrollSpeedNum->setState(static_cast<float>(appConfig.scrollSpeed));

        // Clock duration (1-300 seconds)
        buildEntityId(timingDescs[2].idTemplate, macStr, timeDurID, sizeof(timeDurID));
        timeDurationNum = new HANumber(timeDurID);
        timeDurationNum->setIcon(timingDescs[2].icon);
        timeDurationNum->setName(timingDescs[2].name);
        timeDurationNum->setUnitOfMeasurement(timingDescs[2].unit);
        timeDurationNum->setMin(1);
        timeDurationNum->setMax(300);
        timeDurationNum->setStep(1);
        timeDurationNum->onCommand(onDisplayTimingCommand);
        timeDurationNum->setState(static_cast<float>(appConfig.timeDuration));

        // Date duration (1-60 seconds)
        buildEntityId(timingDescs[3].idTemplate, macStr, dateDurID, sizeof(dateDurID));
        dateDurationNum = new HANumber(dateDurID);
        dateDurationNum->setIcon(timingDescs[3].icon);
        dateDurationNum->setName(timingDescs[3].name);
        dateDurationNum->setUnitOfMeasurement(timingDescs[3].unit);
        dateDurationNum->setMin(1);
        dateDurationNum->setMax(60);
        dateDurationNum->setStep(1);
        dateDurationNum->onCommand(onDisplayTimingCommand);
        dateDurationNum->setState(static_cast<float>(appConfig.dateDuration));

        // Native app color lights (RGB only)
        size_t colorCount;
        const auto *colorDescs = getNativeAppColorDescriptors(colorCount);

        auto createColorLight = [&](HALight *& light, const HAEntityDescriptor& desc,
                                    char *idBuf, size_t idBufSize, uint32_t color)
        {
            buildEntityId(desc.idTemplate, macStr, idBuf, idBufSize);
            light = new HALight(idBuf, HALight::RGBFeature);
            light->setIcon(desc.icon);
            light->setName(desc.name);
            light->onRGBColorCommand(onNativeAppColorCommand);
            HALight::RGBColor c;
            c.isSet = true;
            c.red = (color >> 16) & 0xFF;
            c.green = (color >> 8) & 0xFF;
            c.blue = color & 0xFF;
            light->setCurrentRGBColor(c);
            light->setCurrentState(true);
        };

        createColorLight(timeColorLight, colorDescs[0], timeColID, sizeof(timeColID), colorConfig.timeColor);
        createColorLight(dateColorLight, colorDescs[1], dateColID, sizeof(dateColID), colorConfig.dateColor);
        createColorLight(tempColorLight, colorDescs[2], tempColID, sizeof(tempColID), colorConfig.tempColor);
        createColorLight(humColorLight, colorDescs[3], humColID, sizeof(humColID), colorConfig.humColor);
        createColorLight(batColorLight, colorDescs[4], batColID, sizeof(batColID), colorConfig.batColor);

        // Weather app visibility switches
        size_t weatherVisCount;
        const auto *weatherVisDescs = getWeatherVisibilityDescriptors(weatherVisCount);

        buildEntityId(weatherVisDescs[0].idTemplate, macStr, showOutTempID, sizeof(showOutTempID));
        showOutTempSwitch = new HASwitch(showOutTempID);
        showOutTempSwitch->setIcon(weatherVisDescs[0].icon);
        showOutTempSwitch->setName(weatherVisDescs[0].name);
        showOutTempSwitch->onCommand(onWeatherVisibilitySwitchCommand);
        showOutTempSwitch->setState(weatherConfig.showOutdoorTemp, true);

        buildEntityId(weatherVisDescs[1].idTemplate, macStr, showOutHumID, sizeof(showOutHumID));
        showOutHumSwitch = new HASwitch(showOutHumID);
        showOutHumSwitch->setIcon(weatherVisDescs[1].icon);
        showOutHumSwitch->setName(weatherVisDescs[1].name);
        showOutHumSwitch->onCommand(onWeatherVisibilitySwitchCommand);
        showOutHumSwitch->setState(weatherConfig.showOutdoorHumidity, true);

        buildEntityId(weatherVisDescs[2].idTemplate, macStr, showPressID, sizeof(showPressID));
        showPressureSwitch = new HASwitch(showPressID);
        showPressureSwitch->setIcon(weatherVisDescs[2].icon);
        showPressureSwitch->setName(weatherVisDescs[2].name);
        showPressureSwitch->onCommand(onWeatherVisibilitySwitchCommand);
        showPressureSwitch->setState(weatherConfig.showPressure, true);

        buildEntityId(weatherVisDescs[3].idTemplate, macStr, showAqiID, sizeof(showAqiID));
        showAqiSwitch = new HASwitch(showAqiID);
        showAqiSwitch->setIcon(weatherVisDescs[3].icon);
        showAqiSwitch->setName(weatherVisDescs[3].name);
        showAqiSwitch->onCommand(onWeatherVisibilitySwitchCommand);
        showAqiSwitch->setState(weatherConfig.showAirQuality, true);

        buildEntityId(weatherVisDescs[4].idTemplate, macStr, showUvID, sizeof(showUvID));
        showUvSwitch = new HASwitch(showUvID);
        showUvSwitch->setIcon(weatherVisDescs[4].icon);
        showUvSwitch->setName(weatherVisDescs[4].name);
        showUvSwitch->onCommand(onWeatherVisibilitySwitchCommand);
        showUvSwitch->setState(weatherConfig.showUV, true);

        // Alarm entities (autonomous mode)
        size_t alarmCount;
        const auto *alarmDescs = getAlarmDescriptors(alarmCount);

        buildEntityId(alarmDescs[0].idTemplate, macStr, alarmRingID, sizeof(alarmRingID));
        alarmRinging = new HABinarySensor(alarmRingID);
        alarmRinging->setIcon(alarmDescs[0].icon);
        alarmRinging->setName(alarmDescs[0].name);
        alarmRinging->setState(AlarmManager.isRinging(), true);

        buildEntityId(alarmDescs[1].idTemplate, macStr, alarmSnoozeID, sizeof(alarmSnoozeID));
        alarmSnoozeBtn = new HAButton(alarmSnoozeID);
        alarmSnoozeBtn->setIcon(alarmDescs[1].icon);
        alarmSnoozeBtn->setName(alarmDescs[1].name);
        alarmSnoozeBtn->onCommand(onAlarmButtonCommand);

        buildEntityId(alarmDescs[2].idTemplate, macStr, alarmDismissID, sizeof(alarmDismissID));
        alarmDismissBtn = new HAButton(alarmDismissID);
        alarmDismissBtn->setIcon(alarmDescs[2].icon);
        alarmDismissBtn->setName(alarmDescs[2].name);
        alarmDismissBtn->onCommand(onAlarmButtonCommand);

        buildEntityId(alarmDescs[3].idTemplate, macStr, alarmNextID, sizeof(alarmNextID));
        nextAlarmSensor = new HASensor(alarmNextID);
        nextAlarmSensor->setIcon(alarmDescs[3].icon);
        nextAlarmSensor->setName(alarmDescs[3].name);
    }
    else
    {
        Serial.println(F("Homeassistant discovery disabled"));
        mqtt.disableHA();
    }

    connect();
}
