/**
 * @file HADiscovery.cpp
 * @brief Home Assistant entity descriptors and ID generation.
 *
 * Self-contained string constants — no Dictionary.h dependency.
 * On ESP32, PROGMEM is a no-op (flash is memory-mapped), so
 * using plain static const char[] has zero runtime cost.
 */

#include "HADiscovery.h"
#include <cstdio>

// ── Device info ─────────────────────────────────────────────────────

static const char MANUFACTURER[] = "Blueforcer";
static const char MODEL[] = "SVITRIX";

// ── Matrix light ────────────────────────────────────────────────────

static const char MAT_ID[] = "%s_mat";
static const char MAT_ICON[] = "mdi:clock-digital";
static const char MAT_NAME[] = "Matrix";

// ── Indicator lights ────────────────────────────────────────────────

static const char IND1_ID[] = "%s_ind1";
static const char IND1_ICON[] = "mdi:arrow-top-right-thick";
static const char IND1_NAME[] = "Indicator 1";

static const char IND2_ID[] = "%s_ind2";
static const char IND2_ICON[] = "mdi:arrow-right-thick";
static const char IND2_NAME[] = "Indicator 2";

static const char IND3_ID[] = "%s_ind3";
static const char IND3_ICON[] = "mdi:arrow-bottom-right-thick";
static const char IND3_NAME[] = "Indicator 3";

// ── Selects ─────────────────────────────────────────────────────────

static const char BRI_ID[] = "%s_bri";
static const char BRI_ICON[] = "mdi:brightness-auto";
static const char BRI_NAME[] = "Brightness mode";
static const char BRI_OPTIONS[] = "Manual;Auto";

static const char EFFECT_ID[] = "%s_eff";
static const char EFFECT_ICON[] = "mdi:auto-fix";
static const char EFFECT_NAME[] = "Transition effect";
static const char EFFECT_OPTIONS[] = "Random;Slide;Dim;Zoom;Rotate;Pixelate;Curtain;Ripple;Blink;Reload;Fade";

// ── Buttons ─────────────────────────────────────────────────────────

static const char BTNA_ID[] = "%s_btna";
static const char BTNA_ICON[] = "mdi:bell-off";
static const char BTNA_NAME[] = "Dismiss notification";

static const char DOUP_ID[] = "%s_doupd";
static const char DOUP_ICON[] = "mdi:update";
static const char DOUP_NAME[] = "Start Update";

static const char BTNB_ID[] = "%s_btnb";
static const char BTNB_ICON[] = "mdi:arrow-right-bold";
static const char BTNB_NAME[] = "Next app";

static const char BTNC_ID[] = "%s_btnc";
static const char BTNC_ICON[] = "mdi:arrow-left-bold";
static const char BTNC_NAME[] = "Previous app";

// ── Switch ──────────────────────────────────────────────────────────

static const char TRANS_ID[] = "%s_tra";
static const char TRANS_ICON[] = "mdi:swap-horizontal";
static const char TRANS_NAME[] = "Transition";

// ── Sensors ─────────────────────────────────────────────────────────

static const char APP_ID[] = "%s_app";
static const char APP_ICON[] = "mdi:apps";
static const char APP_NAME[] = "Current app";

static const char DEV_ID[] = "%s_id";
static const char DEV_ICON[] = "mdi:id-card";
static const char DEV_NAME[] = "Device topic";

static const char TEMP_ID[] = "%s_temp";
static const char TEMP_ICON[] = "mdi:thermometer";
static const char TEMP_NAME[] = "Temperature";
static const char TEMP_CLASS[] = "temperature";
static const char TEMP_UNIT[] = "\xC2\xB0""C"; // °C in UTF-8

static const char HUM_ID[] = "%s_hum";
static const char HUM_ICON[] = "mdi:water-percent";
static const char HUM_NAME[] = "Humidity";
static const char HUM_CLASS[] = "humidity";
static const char HUM_UNIT[] = "%";

static const char LUX_ID[] = "%s_lux";
static const char LUX_ICON[] = "mdi:sun-wireless";
static const char LUX_NAME[] = "Illuminance";
static const char LUX_CLASS[] = "illuminance";
static const char LUX_UNIT[] = "lx";

static const char VER_ID[] = "%s_ver";
static const char VER_NAME[] = "Version";

static const char SIG_ID[] = "%s_sig";
static const char SIG_NAME[] = "WiFi strength";
static const char SIG_CLASS[] = "signal_strength";
static const char SIG_UNIT[] = "dB";

static const char UP_ID[] = "%s_up";
static const char UP_NAME[] = "Uptime";
static const char UP_CLASS[] = "duration";
static const char UP_UNIT[] = "s";

static const char RAM_ID[] = "%s_ram";
static const char RAM_ICON[] = "mdi:memory";
static const char RAM_NAME[] = "Free ram";
static const char RAM_CLASS[] = "data_size";
static const char RAM_UNIT[] = "B";

static const char IPADDR_ID[] = "%s_ip_address";
static const char IPADDR_ICON[] = "mdi:wifi";
static const char IPADDR_NAME[] = "IP Address";

static const char BAT_ID[] = "%s_bat";
static const char BAT_ICON[] = "mdi:battery-90";
static const char BAT_NAME[] = "Battery";
static const char BAT_CLASS[] = "battery";
static const char BAT_UNIT[] = "%";

// ── Binary sensors ──────────────────────────────────────────────────

static const char BTNL_ID[] = "%s_btnL";
static const char BTNL_NAME[] = "Button left";

static const char BTNM_ID[] = "%s_btnM";
static const char BTNM_NAME[] = "Button select";

static const char BTNR_ID[] = "%s_btnR";
static const char BTNR_NAME[] = "Button right";

// ── Static descriptor arrays ────────────────────────────────────────

static const HAEntityDescriptor indicatorDescs[] = {
    {IND1_ID, IND1_NAME, IND1_ICON, nullptr, nullptr},
    {IND2_ID, IND2_NAME, IND2_ICON, nullptr, nullptr},
    {IND3_ID, IND3_NAME, IND3_ICON, nullptr, nullptr},
};

static const HASelectDescriptor selectDescs[] = {
    {{BRI_ID, BRI_NAME, BRI_ICON, nullptr, nullptr}, BRI_OPTIONS},
    {{EFFECT_ID, EFFECT_NAME, EFFECT_ICON, nullptr, nullptr}, EFFECT_OPTIONS},
};

static const HAEntityDescriptor buttonDescs[] = {
    {BTNA_ID, BTNA_NAME, BTNA_ICON, nullptr, nullptr},
    {DOUP_ID, DOUP_NAME, DOUP_ICON, nullptr, nullptr},
    {BTNB_ID, BTNB_NAME, BTNB_ICON, nullptr, nullptr},
    {BTNC_ID, BTNC_NAME, BTNC_ICON, nullptr, nullptr},
};

// Battery is always the last entry — count is adjusted by includeBattery.
static const HAEntityDescriptor sensorDescs[] = {
    {APP_ID,    APP_NAME,    APP_ICON,    nullptr,    nullptr},   // 0
    {DEV_ID,    DEV_NAME,    DEV_ICON,    nullptr,    nullptr},   // 1
    {TEMP_ID,   TEMP_NAME,   TEMP_ICON,   TEMP_CLASS, TEMP_UNIT}, // 2
    {HUM_ID,    HUM_NAME,    HUM_ICON,    HUM_CLASS,  HUM_UNIT},  // 3
    {LUX_ID,    LUX_NAME,    LUX_ICON,    LUX_CLASS,  LUX_UNIT},  // 4
    {VER_ID,    VER_NAME,    nullptr,      nullptr,    nullptr},   // 5
    {SIG_ID,    SIG_NAME,    nullptr,      SIG_CLASS,  SIG_UNIT},  // 6
    {UP_ID,     UP_NAME,     nullptr,      UP_CLASS,   UP_UNIT},   // 7
    {RAM_ID,    RAM_NAME,    RAM_ICON,     RAM_CLASS,  RAM_UNIT},  // 8
    {IPADDR_ID, IPADDR_NAME, IPADDR_ICON,  nullptr,    nullptr},   // 9
    {BAT_ID,    BAT_NAME,    BAT_ICON,     BAT_CLASS,  BAT_UNIT},  // 10 (conditional)
};

static constexpr size_t SENSOR_COUNT_NO_BAT = 10;
static constexpr size_t SENSOR_COUNT_ALL = 11;

static const HAEntityDescriptor binarySensorDescs[] = {
    {BTNL_ID, BTNL_NAME, nullptr, nullptr, nullptr},
    {BTNM_ID, BTNM_NAME, nullptr, nullptr, nullptr},
    {BTNR_ID, BTNR_NAME, nullptr, nullptr, nullptr},
};

// ── Function implementations ────────────────────────────────────────

HAEntityDescriptor getMatrixLightDescriptor()
{
    return {MAT_ID, MAT_NAME, MAT_ICON, nullptr, nullptr};
}

const HAEntityDescriptor *getIndicatorLightDescriptors(size_t &count)
{
    count = 3;
    return indicatorDescs;
}

const HASelectDescriptor *getSelectDescriptors(size_t &count)
{
    count = 2;
    return selectDescs;
}

const HAEntityDescriptor *getButtonDescriptors(size_t &count)
{
    count = 4;
    return buttonDescs;
}

HAEntityDescriptor getTransitionSwitchDescriptor()
{
    return {TRANS_ID, TRANS_NAME, TRANS_ICON, nullptr, nullptr};
}

const HAEntityDescriptor *getSensorDescriptors(size_t &count, bool includeBattery)
{
    count = includeBattery ? SENSOR_COUNT_ALL : SENSOR_COUNT_NO_BAT;
    return sensorDescs;
}

const HAEntityDescriptor *getBinarySensorDescriptors(size_t &count)
{
    count = 3;
    return binarySensorDescs;
}

void buildEntityId(const char *idTemplate, const char *macStr,
                   char *outBuf, size_t bufSize)
{
    snprintf(outBuf, bufSize, idTemplate, macStr);
}

void buildConfigUrl(const uint8_t ip[4], char *outBuf, size_t bufSize)
{
    snprintf(outBuf, bufSize, "http://%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
}

const char *getDeviceManufacturer()
{
    return MANUFACTURER;
}

const char *getDeviceModel()
{
    return MODEL;
}

size_t getTotalEntityCount(bool includeBattery)
{
    // 1 matrix + 3 indicators + 2 selects + 4 buttons + 1 switch
    // + sensors (10 or 11) + 3 binary sensors
    return 1 + 3 + 2 + 4 + 1 + (includeBattery ? 11 : 10) + 3;
}
