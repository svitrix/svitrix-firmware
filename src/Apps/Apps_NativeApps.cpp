/**
 * @file Apps_NativeApps.cpp
 * @brief Built-in native app render functions.
 *
 * Contains all five native display apps (Time, Date, Temperature,
 * Humidity, Battery) and their supporting data (big-digit bitmasks,
 * binary clock colors, time format helper).
 */
#include "Apps_internal.h"
#include "Functions.h"
#include "icons.h"
#include "LayoutEngine.h"
#include <LittleFS.h>

// ── Big-digit clock data ───────────────────────────────────────────

/// Bitmask glyphs for big-digit clock (mode 5): digits 0–9, colon, blank.
const uint8_t bigdigits_mask[12][7] = {
    {132, 48, 48, 48, 48, 48, 132},      // 0
    {204, 140, 204, 204, 204, 204, 0},   // 1
    {132, 48, 240, 196, 156, 48, 0},     // 2
    {132, 48, 240, 196, 240, 48, 132},   // 3
    {228, 196, 132, 36, 0, 228, 192},    // 4
    {0, 60, 4, 240, 240, 48, 132},       // 5
    {196, 156, 60, 4, 48, 48, 132},      // 6
    {0, 48, 240, 228, 204, 204, 204},    // 7
    {132, 48, 48, 132, 48, 48, 132},     // 8
    {132, 48, 48, 128, 240, 228, 140},   // 9
    {252, 204, 204, 252, 204, 204, 252}, // :
    {252, 252, 252, 252, 252, 252, 252}  // ; (blank)
};

// ── Binary clock colors ────────────────────────────────────────────

uint32_t COLOR_HOUR_ON = 0xFF0000;   ///< Binary clock hour bit color (red)
uint32_t COLOR_MINUTE_ON = 0x00FF00; ///< Binary clock minute bit color (green)
uint32_t COLOR_SECOND_ON = 0x0000FF; ///< Binary clock second bit color (blue)
uint32_t COLOR_OFF = 0xFFFFFF;       ///< Binary clock off bit color (white)

// ── Time format helper ─────────────────────────────────────────────

/// Return the strftime format string for the current time mode.
/// Modes >0 truncate seconds; blinking separator uses space at position 2.
const char *getTimeFormat()
{
    if (timeConfig.timeMode == 0)
    {
        return timeConfig.timeFormat.c_str();
    }
    else
    {
        if (timeConfig.timeFormat.length() > 5)
        {
            return timeConfig.timeFormat[2] == ' ' ? "%H %M" : "%H:%M";
        }
        else
        {
            return timeConfig.timeFormat.c_str();
        }
    }
}

// ── TimeApp ────────────────────────────────────────────────────────

/// Native clock app with 7 display modes:
///   Mode 5: big-digit GIF background clock
///   Mode 6: binary clock (H/M/S as 6-bit rows)
///   Modes 0–4: text clock with optional calendar box and weekday bar
void TimeApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    if (nativeAppGuard("Time"))
        return;

    const char *timeformat = getTimeFormat();
    static File BIGTIME_BG_GIF;
    static bool BIGTIME_BG_ISGIF = false;
    if (BIGTIME_BG_GIF && timeConfig.timeMode != 5)
    {
        BIGTIME_BG_GIF.close();
        BIGTIME_BG_ISGIF = false;
    }

    // Mode 5: big-digit clock with GIF background
    if (timeConfig.timeMode == 5)
    {
        char t[20];
        strftime(t, sizeof(t), timeformat, timer_localtime());

        static bool bigtimeChecked = false;
        if (!BIGTIME_BG_ISGIF && !bigtimeChecked)
        {
            bigtimeChecked = true;
            if (LittleFS.exists("/bigtime.gif"))
            {
                BIGTIME_BG_GIF = LittleFS.open("/bigtime.gif");
                BIGTIME_BG_ISGIF = true;
            }
        }

        if (BIGTIME_BG_ISGIF)
        {
            static uint16_t BIGTIME_BG_CURRENTFRAME = 0;
            gifPlayer->playGif(0 + x, 0 + y, &BIGTIME_BG_GIF, BIGTIME_BG_CURRENTFRAME);
            BIGTIME_BG_CURRENTFRAME = gifPlayer->getFrame();
        }
        else
        {
            DisplayManager.drawFilledRect(0 + x, 0 + y, 32, 8, colorConfig.textColor);
        }

        t[2] = (timeformat[2] == ' ' && timer_time() % 2) ? ';' : ':';
        t[0] = t[0] == ' ' ? ';' : t[0];
        for (int i = 0; i < 5; i++)
        {
            int idx = t[i] - '0';
            if (idx < 0 || idx > 11)
                idx = 11; // blank for any unexpected character
            int xx = i * 7 - (i > 2 ? 2 : 0) - (i == 2);
            matrix->drawBitmap(xx + x, y, bigdigits_mask[idx], 6, 7, 0);
        }

        matrix->drawFastHLine(0 + x, 7 + y, 32, 0);
        matrix->drawFastVLine(6 + x, 0 + y, 7, 0);
        matrix->drawFastVLine(25 + x, 0 + y, 7, 0);
        return;
    }

    // Mode 6: binary clock
    if (timeConfig.timeMode == 6)
    {
        const struct tm *currentTime = timer_localtime();
        int hour = currentTime->tm_hour;
        int minute = currentTime->tm_min;
        int second = currentTime->tm_sec;

        auto drawBit = [&](int bit, int x1, int y1, uint32_t colorOn, uint32_t colorOff)
        {
            uint32_t color = bit ? colorOn : colorOff;
            for (int dx = 0; dx < 2; dx++)
            {
                for (int dy = 0; dy < 2; dy++)
                {
                    matrix->drawPixel(x1 + dx + x, y1 + dy + y, color);
                }
            }
        };

        for (int i = 0; i < 6; i++)
        {
            int bitValue = (hour >> (5 - i)) & 1;
            drawBit(bitValue, 5 + i * 4 + x, 0 + y, COLOR_HOUR_ON, COLOR_OFF);
        }

        for (int i = 0; i < 6; i++)
        {
            int bitValue = (minute >> (5 - i)) & 1;
            drawBit(bitValue, 5 + i * 4 + x, 3 + y, COLOR_MINUTE_ON, COLOR_OFF);
        }

        for (int i = 0; i < 6; i++)
        {
            int bitValue = (second >> (5 - i)) & 1;
            drawBit(bitValue, 5 + i * 4 + x, 6 + y, COLOR_SECOND_ON, COLOR_OFF);
        }

        return;
    }

    // Modes 0–4: text clock with optional calendar and weekday bar
    applyNativeAppColor(colorConfig.timeColor);

    char t[20];
    if (timeformat[2] == ' ')
    {
        char t2[20];
        strcpy(t2, timeformat);
        if (timer_time() % 2)
        {
            t2[2] = ' ';
        }
        else
        {
            t2[2] = ':';
        }
        strftime(t, sizeof(t), t2, timer_localtime());
    }
    else
    {
        strftime(t, sizeof(t), timeformat, timer_localtime());
    }

    int16_t wdPosY;
    int16_t timePosY;
    if (timeConfig.timeMode == 2 || timeConfig.timeMode == 4)
    {
        wdPosY = 0;
        timePosY = 7;
    }
    else
    {
        wdPosY = 7;
        timePosY = 6;
    }

    DisplayManager.printText(12 + x, timePosY + y, t, timeConfig.timeMode == 0, 2);

    if (timeConfig.timeMode > 0)
    {
        int offset;
        char day_str[3];
        snprintf(day_str, sizeof(day_str), "%d", timer_localtime()->tm_mday);

        DisplayManager.drawFilledRect(x, y, 9, 8, colorConfig.calendarBodyColor);
        if (timeConfig.timeMode <= 2)
        {
            DisplayManager.drawFilledRect(x, y, 9, 2, colorConfig.calendarHeaderColor);
        }
        else
        {
            DisplayManager.drawLine(1 + x, 0 + y, 2 + x, 0 + y, 0x000000);
            DisplayManager.drawLine(6 + x, 0 + y, 7 + x, 0 + y, 0x000000);
        }

        if (timer_localtime()->tm_mday < 10)
            offset = 3;
        else
            offset = 1;
        DisplayManager.setCursor(offset + x, 7 + y);
        DisplayManager.setTextColor(colorConfig.calendarTextColor);
        DisplayManager.matrixPrint(day_str);
    }

    if (!appConfig.showWeekday)
        return;

    uint8_t LINE_WIDTH = timeConfig.timeMode > 0 ? 2 : 3;
    uint8_t LINE_START = timeConfig.timeMode > 0 ? 10 : 2;
    drawWeekdayBar(x, wdPosY + y, LINE_WIDTH, 1, LINE_START);
}

// ── DateApp ────────────────────────────────────────────────────────

/// Native date app showing formatted date with weekday indicator bar.
void DateApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    if (nativeAppGuard("Date"))
        return;

    applyNativeAppColor(colorConfig.dateColor);

    char d[20];
    strftime(d, sizeof(d), timeConfig.dateFormat.c_str(), timer_localtime());
    DisplayManager.printText(0 + x, 6 + y, d, true, 2);

    if (!appConfig.showWeekday)
        return;

    drawWeekdayBar(x, y + 7, 3, 1, 2);
}

// ── TempApp ────────────────────────────────────────────────────────

/// Native temperature app showing sensor reading with thermometer icon.
void TempApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    if (nativeAppGuard("Temperature"))
        return;

    applyNativeAppColor(colorConfig.tempColor);

    LayoutMetrics m = LayoutEngine::computeLayout(appConfig.nativeIconLayout, 0);

    if (m.hasIcon)
    {
        matrix->drawRGBBitmap(x + m.iconX, y, icon_234, 8, 8);
    }

    String tempStr;
    if (timeConfig.isCelsius)
    {
        tempStr = String(sensorConfig.currentTemp, timeConfig.tempDecimalPlaces) + "°C";
    }
    else
    {
        double tempF = (sensorConfig.currentTemp * 9 / 5) + 32;
        tempStr = String(tempF, timeConfig.tempDecimalPlaces) + "°F";
    }

    uint16_t textWidth = getTextWidth(tempStr.c_str(), 0);
    LayoutMetrics tm = LayoutEngine::computeLayout(appConfig.nativeIconLayout, textWidth);
    int16_t textX = tm.textCenterX;

    DisplayManager.setCursor(textX + x, 6 + y);
    DisplayManager.matrixPrint(tempStr.c_str());
}

// ── HumApp ─────────────────────────────────────────────────────────

/// Native humidity app showing sensor reading with droplet icon.
void HumApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    if (nativeAppGuard("Humidity"))
        return;

    applyNativeAppColor(colorConfig.humColor);

    LayoutMetrics m = LayoutEngine::computeLayout(appConfig.nativeIconLayout, 0);

    if (m.hasIcon)
    {
        matrix->drawRGBBitmap(x + m.iconX, y + 1, icon_2075, 8, 8);
    }

    String humStr = String(static_cast<int>(sensorConfig.currentHum)) + "%";
    uint16_t textWidth = getTextWidth(humStr.c_str(), 0);
    LayoutMetrics tm = LayoutEngine::computeLayout(appConfig.nativeIconLayout, textWidth);
    int16_t textX = tm.textCenterX;

    DisplayManager.setCursor(textX + x, 6 + y);
    DisplayManager.matrixPrint(humStr.c_str());
}

// ── BatApp ─────────────────────────────────────────────────────────

/// Native battery app showing charge percentage with battery icon (ULANZI only).
void BatApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    if (nativeAppGuard("Battery"))
        return;

    applyNativeAppColor(colorConfig.batColor);

    LayoutMetrics m = LayoutEngine::computeLayout(appConfig.nativeIconLayout, 0);

    if (m.hasIcon)
    {
        matrix->drawRGBBitmap(x + m.iconX, y, icon_1486, 8, 8);
    }

    String batStr = String(static_cast<int>(batteryConfig.percent)) + "%";
    uint16_t textWidth = getTextWidth(batStr.c_str(), 0);
    LayoutMetrics tm = LayoutEngine::computeLayout(appConfig.nativeIconLayout, textWidth);
    int16_t textX = tm.textCenterX;

    DisplayManager.setCursor(textX + x, 6 + y);
    DisplayManager.matrixPrint(batStr.c_str());
}
