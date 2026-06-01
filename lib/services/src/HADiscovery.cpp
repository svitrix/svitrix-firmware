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
static const char EFFECT_OPTIONS[] = "Random;Slide Down;Dim;Zoom;Rotate;Pixelate;Curtain;Ripple;Blink;Reload;Fade;Slide Up;Slide Left;Slide Right";

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

static const char REBOOT_ID[] = "%s_reboot";
static const char REBOOT_ICON[] = "mdi:restart";
static const char REBOOT_NAME[] = "Reboot";

static const char PLAYSOUND_ID[] = "%s_play_sound";
static const char PLAYSOUND_ICON[] = "mdi:play-circle";
static const char PLAYSOUND_NAME[] = "Play test sound";

// ── Audio controls ──────────────────────────────────────────────────

static const char SOUND_EN_ID[] = "%s_sound_enabled";
static const char SOUND_EN_ICON[] = "mdi:volume-high";
static const char SOUND_EN_NAME[] = "Sound enabled";

static const char SOUND_VOL_ID[] = "%s_sound_volume";
static const char SOUND_VOL_ICON[] = "mdi:volume-medium";
static const char SOUND_VOL_NAME[] = "Sound volume";

// ── App visibility ─────────────────────────────────────────────────

static const char SHOW_TIME_ID[] = "%s_show_time";
static const char SHOW_TIME_ICON[] = "mdi:clock-outline";
static const char SHOW_TIME_NAME[] = "Show time app";

static const char SHOW_DATE_ID[] = "%s_show_date";
static const char SHOW_DATE_ICON[] = "mdi:calendar";
static const char SHOW_DATE_NAME[] = "Show date app";

static const char SHOW_TEMP_ID[] = "%s_show_temp";
static const char SHOW_TEMP_ICON[] = "mdi:thermometer";
static const char SHOW_TEMP_NAME[] = "Show temperature app";

static const char SHOW_HUM_ID[] = "%s_show_hum";
static const char SHOW_HUM_ICON[] = "mdi:water-percent";
static const char SHOW_HUM_NAME[] = "Show humidity app";

static const char SHOW_BAT_ID[] = "%s_show_bat";
static const char SHOW_BAT_ICON[] = "mdi:battery";
static const char SHOW_BAT_NAME[] = "Show battery app";

// ── Background effect ──────────────────────────────────────────────

static const char BGEFFECT_ID[] = "%s_bg_effect";
static const char BGEFFECT_ICON[] = "mdi:auto-fix";
static const char BGEFFECT_NAME[] = "Background effect";
static const char BGEFFECT_OPTIONS[] = "None;Fade;MovingLine;BrickBreaker;PingPong;Radar;Checkerboard;Fireworks;PlasmaCloud;Ripple;Snake;Pacifica;TheaterChase;Plasma;Matrix;SwirlIn;SwirlOut;LookingEyes;TwinklingStars;ColorWaves;Fire";

// ── Display timing ─────────────────────────────────────────────────

static const char TIMEPERAPP_ID[] = "%s_time_per_app";
static const char TIMEPERAPP_ICON[] = "mdi:timer-sand";
static const char TIMEPERAPP_NAME[] = "Time per app";
static const char TIMEPERAPP_UNIT[] = "s";

static const char SCROLLSPEED_ID[] = "%s_scroll_speed";
static const char SCROLLSPEED_ICON[] = "mdi:speedometer";
static const char SCROLLSPEED_NAME[] = "Scroll speed";
static const char SCROLLSPEED_UNIT[] = "ms";

static const char TIMEDUR_ID[] = "%s_time_duration";
static const char TIMEDUR_ICON[] = "mdi:clock-outline";
static const char TIMEDUR_NAME[] = "Clock duration";
static const char TIMEDUR_UNIT[] = "s";

static const char DATEDUR_ID[] = "%s_date_duration";
static const char DATEDUR_ICON[] = "mdi:calendar";
static const char DATEDUR_NAME[] = "Date duration";
static const char DATEDUR_UNIT[] = "s";

static const char TEMPDUR_ID[] = "%s_temp_duration";
static const char TEMPDUR_ICON[] = "mdi:thermometer";
static const char TEMPDUR_NAME[] = "Temperature duration";
static const char TEMPDUR_UNIT[] = "s";

static const char HUMDUR_ID[] = "%s_hum_duration";
static const char HUMDUR_ICON[] = "mdi:water-percent";
static const char HUMDUR_NAME[] = "Humidity duration";
static const char HUMDUR_UNIT[] = "s";

static const char BATDUR_ID[] = "%s_bat_duration";
static const char BATDUR_ICON[] = "mdi:battery";
static const char BATDUR_NAME[] = "Battery duration";
static const char BATDUR_UNIT[] = "s";

static const char OUTTEMPDUR_ID[] = "%s_out_temp_duration";
static const char OUTTEMPDUR_ICON[] = "mdi:thermometer";
static const char OUTTEMPDUR_NAME[] = "Outdoor temp duration";
static const char OUTTEMPDUR_UNIT[] = "s";

static const char OUTHUMDUR_ID[] = "%s_out_hum_duration";
static const char OUTHUMDUR_ICON[] = "mdi:water-percent";
static const char OUTHUMDUR_NAME[] = "Outdoor humidity duration";
static const char OUTHUMDUR_UNIT[] = "s";

static const char PRESSDUR_ID[] = "%s_pressure_duration";
static const char PRESSDUR_ICON[] = "mdi:gauge";
static const char PRESSDUR_NAME[] = "Pressure duration";
static const char PRESSDUR_UNIT[] = "s";

static const char AQIDUR_ID[] = "%s_aqi_duration";
static const char AQIDUR_ICON[] = "mdi:air-filter";
static const char AQIDUR_NAME[] = "Air quality duration";
static const char AQIDUR_UNIT[] = "s";

static const char UVDUR_ID[] = "%s_uv_duration";
static const char UVDUR_ICON[] = "mdi:weather-sunny-alert";
static const char UVDUR_NAME[] = "UV index duration";
static const char UVDUR_UNIT[] = "s";

// ── Weather app visibility ─────────────────────────────────────────

static const char SHOW_OUTTEMP_ID[] = "%s_show_out_temp";
static const char SHOW_OUTTEMP_ICON[] = "mdi:thermometer";
static const char SHOW_OUTTEMP_NAME[] = "Show outdoor temp";

static const char SHOW_OUTHUM_ID[] = "%s_show_out_hum";
static const char SHOW_OUTHUM_ICON[] = "mdi:water-percent";
static const char SHOW_OUTHUM_NAME[] = "Show outdoor humidity";

static const char SHOW_PRESS_ID[] = "%s_show_pressure";
static const char SHOW_PRESS_ICON[] = "mdi:gauge";
static const char SHOW_PRESS_NAME[] = "Show pressure";

static const char SHOW_AQI_ID[] = "%s_show_aqi";
static const char SHOW_AQI_ICON[] = "mdi:air-filter";
static const char SHOW_AQI_NAME[] = "Show air quality";

static const char SHOW_UV_ID[] = "%s_show_uv";
static const char SHOW_UV_ICON[] = "mdi:sun-wireless-outline";
static const char SHOW_UV_NAME[] = "Show UV index";

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

// ── Weather sensors (from WeatherAPI) ───────────────────────────────

static const char OUTTEMP_ID[] = "%s_out_temp";
static const char OUTTEMP_ICON[] = "mdi:thermometer";
static const char OUTTEMP_NAME[] = "Outdoor Temperature";
static const char OUTTEMP_CLASS[] = "temperature";

static const char OUTHUM_ID[] = "%s_out_hum";
static const char OUTHUM_ICON[] = "mdi:water-percent";
static const char OUTHUM_NAME[] = "Outdoor Humidity";
static const char OUTHUM_CLASS[] = "humidity";
static const char OUTHUM_UNIT[] = "%";

static const char PRESS_ID[] = "%s_pressure";
static const char PRESS_ICON[] = "mdi:gauge";
static const char PRESS_NAME[] = "Pressure";
static const char PRESS_CLASS[] = "pressure";
static const char PRESS_UNIT[] = "hPa";

static const char AQI_ID[] = "%s_aqi";
static const char AQI_ICON[] = "mdi:air-filter";
static const char AQI_NAME[] = "Air Quality Index";
static const char AQI_CLASS[] = "aqi";

static const char WCOND_ID[] = "%s_weather_cond";
static const char WCOND_ICON[] = "mdi:weather-partly-cloudy";
static const char WCOND_NAME[] = "Weather Condition";

static const char UV_ID[] = "%s_uv";
static const char UV_ICON[] = "mdi:sun-wireless-outline";
static const char UV_NAME[] = "UV Index";

// ── Night mode ──────────────────────────────────────────────────────

static const char NMODE_ID[] = "%s_night_mode";
static const char NMODE_ICON[] = "mdi:weather-night";
static const char NMODE_NAME[] = "Night mode";

static const char NBRI_ID[] = "%s_night_brightness";
static const char NBRI_ICON[] = "mdi:brightness-4";
static const char NBRI_NAME[] = "Night brightness";

static const char NCOL_ID[] = "%s_night_color";
static const char NCOL_ICON[] = "mdi:palette";
static const char NCOL_NAME[] = "Night color";

static const char NBLOCK_ID[] = "%s_night_block_trans";
static const char NBLOCK_ICON[] = "mdi:swap-horizontal-bold";
static const char NBLOCK_NAME[] = "Night block transition";

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
    {REBOOT_ID, REBOOT_NAME, REBOOT_ICON, nullptr, nullptr},
    {PLAYSOUND_ID, PLAYSOUND_NAME, PLAYSOUND_ICON, nullptr, nullptr},
};

// Audio controls array (2 entities: 1 switch, 1 number)
static const HAEntityDescriptor audioDescs[] = {
    {SOUND_EN_ID, SOUND_EN_NAME, SOUND_EN_ICON, nullptr, nullptr},  // 0 - switch
    {SOUND_VOL_ID, SOUND_VOL_NAME, SOUND_VOL_ICON, nullptr, nullptr}, // 1 - number
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

// Weather sensors array (6 sensors)
static const HAEntityDescriptor weatherSensorDescs[] = {
    {OUTTEMP_ID, OUTTEMP_NAME, OUTTEMP_ICON, OUTTEMP_CLASS, nullptr},    // 0 - unit set dynamically (°C/°F)
    {OUTHUM_ID,  OUTHUM_NAME,  OUTHUM_ICON,  OUTHUM_CLASS,  OUTHUM_UNIT}, // 1
    {PRESS_ID,   PRESS_NAME,   PRESS_ICON,   PRESS_CLASS,   PRESS_UNIT},  // 2
    {AQI_ID,     AQI_NAME,     AQI_ICON,     AQI_CLASS,     nullptr},     // 3
    {WCOND_ID,   WCOND_NAME,   WCOND_ICON,   nullptr,       nullptr},     // 4
    {UV_ID,      UV_NAME,      UV_ICON,      nullptr,       nullptr},     // 5
};

// Night mode array (4 entities: 2 switches, 1 number, 1 light)
static const HAEntityDescriptor nightModeDescs[] = {
    {NMODE_ID,  NMODE_NAME,  NMODE_ICON,  nullptr, nullptr}, // 0 - switch
    {NBRI_ID,   NBRI_NAME,   NBRI_ICON,   nullptr, nullptr}, // 1 - number
    {NCOL_ID,   NCOL_NAME,   NCOL_ICON,   nullptr, nullptr}, // 2 - light (RGB)
    {NBLOCK_ID, NBLOCK_NAME, NBLOCK_ICON, nullptr, nullptr}, // 3 - switch
};

// App visibility array (5 switches)
static const HAEntityDescriptor appVisibilityDescs[] = {
    {SHOW_TIME_ID, SHOW_TIME_NAME, SHOW_TIME_ICON, nullptr, nullptr},
    {SHOW_DATE_ID, SHOW_DATE_NAME, SHOW_DATE_ICON, nullptr, nullptr},
    {SHOW_TEMP_ID, SHOW_TEMP_NAME, SHOW_TEMP_ICON, nullptr, nullptr},
    {SHOW_HUM_ID,  SHOW_HUM_NAME,  SHOW_HUM_ICON,  nullptr, nullptr},
    {SHOW_BAT_ID,  SHOW_BAT_NAME,  SHOW_BAT_ICON,  nullptr, nullptr},
};

// Display timing array (12 numbers)
static const HAEntityDescriptor displayTimingDescs[] = {
    {TIMEPERAPP_ID,  TIMEPERAPP_NAME,  TIMEPERAPP_ICON,  nullptr, TIMEPERAPP_UNIT},
    {SCROLLSPEED_ID, SCROLLSPEED_NAME, SCROLLSPEED_ICON, nullptr, SCROLLSPEED_UNIT},
    {TIMEDUR_ID,     TIMEDUR_NAME,     TIMEDUR_ICON,     nullptr, TIMEDUR_UNIT},
    {DATEDUR_ID,     DATEDUR_NAME,     DATEDUR_ICON,     nullptr, DATEDUR_UNIT},
    {TEMPDUR_ID,     TEMPDUR_NAME,     TEMPDUR_ICON,     nullptr, TEMPDUR_UNIT},
    {HUMDUR_ID,      HUMDUR_NAME,      HUMDUR_ICON,      nullptr, HUMDUR_UNIT},
    {BATDUR_ID,      BATDUR_NAME,      BATDUR_ICON,      nullptr, BATDUR_UNIT},
    {OUTTEMPDUR_ID,  OUTTEMPDUR_NAME,  OUTTEMPDUR_ICON,  nullptr, OUTTEMPDUR_UNIT},
    {OUTHUMDUR_ID,   OUTHUMDUR_NAME,   OUTHUMDUR_ICON,   nullptr, OUTHUMDUR_UNIT},
    {PRESSDUR_ID,    PRESSDUR_NAME,    PRESSDUR_ICON,    nullptr, PRESSDUR_UNIT},
    {AQIDUR_ID,      AQIDUR_NAME,      AQIDUR_ICON,      nullptr, AQIDUR_UNIT},
    {UVDUR_ID,       UVDUR_NAME,       UVDUR_ICON,       nullptr, UVDUR_UNIT},
};

// Weather app visibility array (5 switches)
static const HAEntityDescriptor weatherVisibilityDescs[] = {
    {SHOW_OUTTEMP_ID, SHOW_OUTTEMP_NAME, SHOW_OUTTEMP_ICON, nullptr, nullptr},
    {SHOW_OUTHUM_ID,  SHOW_OUTHUM_NAME,  SHOW_OUTHUM_ICON,  nullptr, nullptr},
    {SHOW_PRESS_ID,   SHOW_PRESS_NAME,   SHOW_PRESS_ICON,   nullptr, nullptr},
    {SHOW_AQI_ID,     SHOW_AQI_NAME,     SHOW_AQI_ICON,     nullptr, nullptr},
    {SHOW_UV_ID,      SHOW_UV_NAME,      SHOW_UV_ICON,      nullptr, nullptr},
};

// ── Alarm entities (autonomous mode) ────────────────────────────────
static const char ALARM_RING_ID[] = "%s_alarm_ring";
static const char ALARM_RING_NAME[] = "Alarm ringing";
static const char ALARM_RING_ICON[] = "mdi:alarm";
static const char ALARM_SNOOZE_ID[] = "%s_alarm_snooze";
static const char ALARM_SNOOZE_NAME[] = "Snooze alarm";
static const char ALARM_SNOOZE_ICON[] = "mdi:alarm-snooze";
static const char ALARM_DISMISS_ID[] = "%s_alarm_dismiss";
static const char ALARM_DISMISS_NAME[] = "Dismiss alarm";
static const char ALARM_DISMISS_ICON[] = "mdi:alarm-off";
static const char ALARM_NEXT_ID[] = "%s_alarm_next";
static const char ALARM_NEXT_NAME[] = "Next alarm";
static const char ALARM_NEXT_ICON[] = "mdi:alarm-check";

// Alarm array (4: binary_sensor ringing, snooze button, dismiss button, next-alarm sensor)
static const HAEntityDescriptor alarmDescs[] = {
    {ALARM_RING_ID,    ALARM_RING_NAME,    ALARM_RING_ICON,    nullptr, nullptr}, // 0 - binary_sensor
    {ALARM_SNOOZE_ID,  ALARM_SNOOZE_NAME,  ALARM_SNOOZE_ICON,  nullptr, nullptr}, // 1 - button
    {ALARM_DISMISS_ID, ALARM_DISMISS_NAME, ALARM_DISMISS_ICON, nullptr, nullptr}, // 2 - button
    {ALARM_NEXT_ID,    ALARM_NEXT_NAME,    ALARM_NEXT_ICON,    nullptr, nullptr}, // 3 - sensor
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
    count = 6;
    return buttonDescs;
}

const HAEntityDescriptor *getAudioDescriptors(size_t &count)
{
    count = 2;
    return audioDescs;
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

const HAEntityDescriptor *getWeatherSensorDescriptors(size_t &count)
{
    count = 6;
    return weatherSensorDescs;
}

const HAEntityDescriptor *getNightModeDescriptors(size_t &count)
{
    count = 4;
    return nightModeDescs;
}

const HAEntityDescriptor *getAppVisibilityDescriptors(size_t &count)
{
    count = 5;
    return appVisibilityDescs;
}

HASelectDescriptor getBackgroundEffectDescriptor()
{
    return {{BGEFFECT_ID, BGEFFECT_NAME, BGEFFECT_ICON, nullptr, nullptr}, BGEFFECT_OPTIONS};
}

const HAEntityDescriptor *getDisplayTimingDescriptors(size_t &count)
{
    count = 12;
    return displayTimingDescs;
}

const HAEntityDescriptor *getWeatherVisibilityDescriptors(size_t &count)
{
    count = 5;
    return weatherVisibilityDescs;
}

const HAEntityDescriptor *getAlarmDescriptors(size_t &count)
{
    count = 4;
    return alarmDescs;
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
    // 1 matrix + 3 indicators + 3 selects + 6 buttons + 1 switch
    // + sensors (10 or 11) + 3 binary + 6 weather + 4 night + 2 audio + 5 app vis + 12 timing + 5 weather vis + 4 alarm
    return 1 + 3 + 3 + 6 + 1 + (includeBattery ? 11 : 10) + 3 + 6 + 4 + 2 + 5 + 12 + 5 + 4;
}
