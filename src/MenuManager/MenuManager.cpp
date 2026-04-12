#include <MenuManager.h>
#include <Arduino.h>
#include <Globals.h>
#include <ServerManager.h>
#include "IDisplayRenderer.h"
#include "IDisplayControl.h"
#include "IDisplayNavigation.h"
#include <cassert>
#include "IPeripheryProvider.h"
#include "IUpdater.h"
#include "timer.h"
#include <icons.h>

enum MenuState
{
    MainMenu,
    BrightnessMenu,
    ColorMenu,
    SwitchMenu,
    TspeedMenu,
    AppTimeMenu,
    TimeFormatMenu,
    DateFormatMenu,
    WeekdayMenu,
    TempMenu,
    Appmenu,
    SoundMenu,
    VolumeMenu,
    UpdateMenu,
    MaxMenu
};

const char *menuItems[] PROGMEM = {
    "BRIGHT",
    "COLOR",
    "SWITCH",
    "T-SPEED",
    "APPTIME",
    "TIME",
    "DATE",
    "WEEKDAY",
    "TEMP",
    "APPS",
    "SOUND",
    "VOLUME",
    "UPDATE"};

int8_t menuIndex = 0;
uint8_t menuItemCount = MaxMenu - 1;

const char *timeFormat[] PROGMEM = {
    "%H:%M:%S",
    "%l:%M:%S",
    "%H:%M",
    "%H %M",
    "%l:%M",
    "%l %M",
    "%l:%M %p",
    "%l %M %p"};
int8_t timeFormatIndex = 0;
uint8_t timeFormatCount = 8;

const char *dateFormat[] PROGMEM = {
    "%d.%m.%y", // 01.04.22
    "%d.%m.",   // 01.04.
    "%y-%m-%d", // 22-04-01
    "%m-%d",    // 04-01
    "%m/%d/%y", // 04/01/22
    "%m/%d",    // 04/01
    "%d/%m/%y", // 01/04/22
    "%d/%m",    // 01/04
    "%m-%d-%y", // 04-01-22
};

int8_t dateFormatIndex = 0;
uint8_t dateFormatCount = 9;

int8_t appsIndex = 0;
static const uint8_t appsCount = 5;

MenuState currentState = MainMenu;

uint32_t textColors[] = {
    0xFFFFFF, // White
    0xFF0000, // Red
    0x00FF00, // Green
    0x0000FF, // Blue
    0xFFFF00, // Yellow
    0xFF00FF, // Magenta
    0x00FFFF, // Cyan
    0xFFA500, // Orange
    0x800080, // Purple
    0x008080, // Teal
    0x808000, // Olive
    0x800000, // Maroon
    0x008000, // Dark Green
    0x000080, // Navy
    0x808080  // Gray
};

uint8_t currentColor = 0;
static const uint8_t COLOR_COUNT = sizeof(textColors) / sizeof(textColors[0]);
static const uint8_t MAX_VOLUME = 30;
static const int MIN_TRANSITION_SPEED = 200;
static const int MAX_TRANSITION_SPEED = 2000;
static const int TRANSITION_SPEED_STEP = 100;
static const long MIN_APP_TIME = 1000;
static const long MAX_APP_TIME = 30000;
static const long APP_TIME_STEP = 1000;

MenuManager_& MenuManager_::getInstance()
{
    static MenuManager_ instance;
    return instance;
}

// Initialize the global shared instance
MenuManager_& MenuManager = MenuManager.getInstance();

void MenuManager_::setDisplay(IDisplayRenderer *r, IDisplayControl *c, IDisplayNavigation *n)
{
    assert(r && c && n);
    renderer_ = r;
    control_ = c;
    nav_ = n;
}

void MenuManager_::setServices(IPeripheryProvider *pp, IUpdater *u)
{
    assert(pp && u);
    periphery_ = pp;
    updater_ = u;
}

int convertBRIPercentTo8Bit(int brightness_percent)
{
    int brightness;
    if (brightness_percent <= 10)
    {
        // Map 10 % or lower 1:1 to 0:255 range. Reasons:
        // * 1% would be mapped to 2 so lowest value would be inaccessible.
        // * Small changes in lower brightness are perceived by humans
        //   as big changes, so it makes sense to give higher
        //   "resolution" here.
        brightness = brightness_percent;
    }
    else
    {
        brightness = map(brightness_percent, 0, 100, 0, 255);
    }
    return brightness;
}

String MenuManager_::menutext()
{
    char buf[24];
    switch (currentState)
    {
    case MainMenu:
        renderer_->drawMenuIndicator(menuIndex, menuItemCount, 0xF80000);
        return menuItems[menuIndex];
    case BrightnessMenu:
        if (brightnessConfig.autoBrightness)
            return "AUTO";
        snprintf(buf, sizeof(buf), "%d%%", brightnessConfig.brightnessPercent);
        return buf;
    case ColorMenu:
        renderer_->drawMenuIndicator(currentColor, COLOR_COUNT, 0xFBC000);
        renderer_->setTextColor(textColors[currentColor]);
        snprintf(buf, sizeof(buf), "0X%X", textColors[currentColor]);
        return buf;
    case SwitchMenu:
        return appConfig.autoTransition ? "ON" : "OFF";
    case SoundMenu:
        return audioConfig.soundActive ? "ON" : "OFF";
    case TspeedMenu:
        snprintf(buf, sizeof(buf), "%.1fs", appConfig.timePerTransition / 1000.0);
        return buf;
    case AppTimeMenu:
        snprintf(buf, sizeof(buf), "%.0fs", appConfig.timePerApp / 1000.0);
        return buf;
    case TimeFormatMenu:
    {
        renderer_->drawMenuIndicator(timeFormatIndex, timeFormatCount, 0xFBC000);
        char display[9];
        if (timeFormat[timeFormatIndex][2] == ' ')
        {
            snprintf(display, sizeof(display), "%s", timeFormat[timeFormatIndex]);
            display[2] = timer_time() % 2 ? ' ' : ':';
        }
        else
        {
            snprintf(display, sizeof(display), "%s", timeFormat[timeFormatIndex]);
        }
        strftime(buf, sizeof(buf), display, timer_localtime());
        return buf;
    }
    case DateFormatMenu:
        renderer_->drawMenuIndicator(dateFormatIndex, dateFormatCount, 0xFBC000);
        strftime(buf, sizeof(buf), dateFormat[dateFormatIndex], timer_localtime());
        return buf;
    case WeekdayMenu:
        return timeConfig.startOnMonday ? "MON" : "SUN";
    case TempMenu:
        return timeConfig.isCelsius ? "°C" : "°F";
    case Appmenu:
        renderer_->drawMenuIndicator(appsIndex, appsCount, 0xFBC000);
        switch (appsIndex)
        {
        case 0:
            renderer_->drawBMP(0, 0, icon_13, 8, 8);
            return appConfig.showTime ? "ON" : "OFF";
        case 1:
            renderer_->drawBMP(0, 0, icon_1158, 8, 8);
            return appConfig.showDate ? "ON" : "OFF";
        case 2:
            renderer_->drawBMP(0, 0, icon_234, 8, 8);
            return appConfig.showTemp ? "ON" : "OFF";
        case 3:
            renderer_->drawBMP(0, 0, icon_2075, 8, 8);
            return appConfig.showHum ? "ON" : "OFF";
        case 4:
            renderer_->drawBMP(0, 0, icon_1486, 8, 8);
            return appConfig.showBat ? "ON" : "OFF";
        default:
            break;
        }
        break;
    case VolumeMenu:
        snprintf(buf, sizeof(buf), "%d", audioConfig.soundVolume);
        return buf;
    default:
        break;
    }
    return "";
}

void MenuManager_::rightButton()
{
    if (!inMenu)
        return;
    switch (currentState)
    {
    case MainMenu:
        menuIndex = (menuIndex + 1) % menuItemCount;
        break;
    case BrightnessMenu:
        if (!brightnessConfig.autoBrightness)
        {
            brightnessConfig.brightnessPercent = (brightnessConfig.brightnessPercent % 100) + 1;
            brightnessConfig.brightness = convertBRIPercentTo8Bit(brightnessConfig.brightnessPercent);
            control_->setBrightness(brightnessConfig.brightness);
        }
        break;
    case ColorMenu:
        currentColor = (currentColor + 1) % COLOR_COUNT;
        break;
    case SwitchMenu:
        appConfig.autoTransition = !appConfig.autoTransition;
        break;
    case TspeedMenu:
        appConfig.timePerTransition = min(MAX_TRANSITION_SPEED, appConfig.timePerTransition + TRANSITION_SPEED_STEP);
        break;
    case AppTimeMenu:
        appConfig.timePerApp = min(MAX_APP_TIME, appConfig.timePerApp + APP_TIME_STEP);
        break;
    case TimeFormatMenu:
        timeFormatIndex = (timeFormatIndex + 1) % timeFormatCount;
        break;
    case DateFormatMenu:
        dateFormatIndex = (dateFormatIndex + 1) % dateFormatCount;
        break;
    case Appmenu:
        appsIndex = (appsIndex + 1) % appsCount;
        break;
    case WeekdayMenu:
        timeConfig.startOnMonday = !timeConfig.startOnMonday;
        break;
    case SoundMenu:
        audioConfig.soundActive = !audioConfig.soundActive;
        break;
    case TempMenu:
        timeConfig.isCelsius = !timeConfig.isCelsius;
        break;
    case VolumeMenu:
        if ((audioConfig.soundVolume + 1) > MAX_VOLUME)
            audioConfig.soundVolume = 0;
        else
            audioConfig.soundVolume++;
        break;
    default:
        break;
    }
}

void MenuManager_::leftButton()
{
    if (!inMenu)
    {
        return;
    }
    switch (currentState)
    {
    case MainMenu:
        menuIndex = (menuIndex == 0) ? menuItemCount - 1 : menuIndex - 1;
        break;
    case BrightnessMenu:
        if (!brightnessConfig.autoBrightness)
        {
            brightnessConfig.brightnessPercent = (brightnessConfig.brightnessPercent == 1) ? 100 : brightnessConfig.brightnessPercent - 1;
            brightnessConfig.brightness = convertBRIPercentTo8Bit(brightnessConfig.brightnessPercent);
            control_->setBrightness(brightnessConfig.brightness);
        }
        break;
    case ColorMenu:
        currentColor = (currentColor + COLOR_COUNT - 1) % COLOR_COUNT;
        break;
    case SwitchMenu:
        appConfig.autoTransition = !appConfig.autoTransition;
        break;
    case TspeedMenu:
        appConfig.timePerTransition = max(MIN_TRANSITION_SPEED, appConfig.timePerTransition - TRANSITION_SPEED_STEP);
        break;
    case AppTimeMenu:
        appConfig.timePerApp = max(MIN_APP_TIME, appConfig.timePerApp - APP_TIME_STEP);
        break;
    case TimeFormatMenu:
        timeFormatIndex = (timeFormatIndex == 0) ? timeFormatCount - 1 : timeFormatIndex - 1;
        break;
    case DateFormatMenu:
        dateFormatIndex = (dateFormatIndex == 0) ? dateFormatCount - 1 : dateFormatIndex - 1;
        break;
    case Appmenu:
        appsIndex = (appsIndex == 0) ? appsCount - 1 : appsIndex - 1;
        break;
    case WeekdayMenu:
        timeConfig.startOnMonday = !timeConfig.startOnMonday;
        break;
    case TempMenu:
        timeConfig.isCelsius = !timeConfig.isCelsius;
        break;
    case SoundMenu:
        audioConfig.soundActive = !audioConfig.soundActive;
        break;
    case VolumeMenu:
        if ((audioConfig.soundVolume - 1) < 0)
            audioConfig.soundVolume = MAX_VOLUME;
        else
            audioConfig.soundVolume--;
        break;
    default:
        break;
    }
}

void MenuManager_::selectButton()
{
    if (!inMenu)
    {
        return;
    }
    switch (currentState)
    {
    case MainMenu:
        currentState = (MenuState)(menuIndex + 1);
        switch (currentState)
        {
        case BrightnessMenu:
            // reverse of convertBRIPercentTo8Bit.
            if (brightnessConfig.brightness <= 10)
            {
                brightnessConfig.brightnessPercent = brightnessConfig.brightness;
            }
            else
            {
                brightnessConfig.brightnessPercent = map(brightnessConfig.brightness, 0, 255, 0, 100);
            }
            break;
        case UpdateMenu:
            if (updater_->checkUpdate(true))
            {
                updater_->updateFirmware();
            }
            break;
        }
        break;
    case BrightnessMenu:
        brightnessConfig.autoBrightness = !brightnessConfig.autoBrightness;
        if (!brightnessConfig.autoBrightness)
        {
            brightnessConfig.brightness = convertBRIPercentTo8Bit(brightnessConfig.brightnessPercent);
            control_->setBrightness(brightnessConfig.brightness);
        }
        break;
    case Appmenu:
        switch (appsIndex)
        {
        case 0:
            appConfig.showTime = !appConfig.showTime;
            break;
        case 1:
            appConfig.showDate = !appConfig.showDate;
            break;
        case 2:
            appConfig.showTemp = !appConfig.showTemp;
            break;
        case 3:
            appConfig.showHum = !appConfig.showHum;
            break;
        case 4:
            appConfig.showBat = !appConfig.showBat;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void MenuManager_::selectButtonLong()
{
    if (inMenu)
    {
        switch (currentState)
        {
        case BrightnessMenu:
            // brightnessConfig.brightness = map(brightnessConfig.brightnessPercent, 0, 100, 0, 255);
            saveSettings();
            break;
        case ColorMenu:
            colorConfig.textColor = textColors[currentColor];
            saveSettings();
            break;
        case MainMenu:
            inMenu = false;
            break;
        case SwitchMenu:
            control_->setAutoTransition(appConfig.autoTransition);
            saveSettings();
            break;
        case TspeedMenu:
        case AppTimeMenu:
            control_->applyAllSettings();
            saveSettings();
            break;
        case TimeFormatMenu:
            timeConfig.timeFormat = timeFormat[timeFormatIndex];
            saveSettings();
            break;
        case DateFormatMenu:
            timeConfig.dateFormat = dateFormat[dateFormatIndex];
            saveSettings();
            break;
        case WeekdayMenu:
        case SoundMenu:
        case TempMenu:
            saveSettings();
            break;
        case Appmenu:
            nav_->loadNativeApps();
            saveSettings();
            break;
        case VolumeMenu:
            periphery_->setVolume(audioConfig.soundVolume);
            saveSettings();
            break;
        default:
            break;
        }
        currentState = MainMenu;
    }
    else
    {
        inMenu = true;
    }
}
