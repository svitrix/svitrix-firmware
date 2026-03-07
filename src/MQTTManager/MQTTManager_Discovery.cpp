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

        dismiss->onCommand(onButtonCommand);
        nextApp->onCommand(onButtonCommand);
        prevApp->onCommand(onButtonCommand);

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
    }
    else
    {
        Serial.println(F("Homeassistant discovery disabled"));
        mqtt.disableHA();
    }

    connect();
}
