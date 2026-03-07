#include <PeripheryManager.h>
#include "IButtonHandler.h"
#include "IButtonReporter.h"
#include "SensorCalc.h"
#include "Adafruit_SHT31.h"
#include "Adafruit_BME280.h"
#include "Adafruit_BMP280.h"
#include "Adafruit_HTU21DF.h"
#include <MelodyPlayer/melody_player.h>
#include <MelodyPlayer/melody_factory.h>
#include "Globals.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <LightDependentResistor.h>
#include <MedianFilterLib.h>
#include <MeanFilterLib.h>
// ── Hardware constants ────────────────────────────────────────────
constexpr int kLedcChannel = 0;
constexpr int kLedcResolution = 8; // 8-bit PWM resolution

constexpr int kMedianWindow = 7; // Median filter window for spike rejection
constexpr int kMeanWindow = 7;   // Mean filter window for averaging

constexpr int kResetPin = 13;

// Pinouts for the Ulanzi TC001
constexpr int kBuzzerPin = 15;
constexpr int kBatteryPin = 34;
constexpr int kLdrPin = 35;
constexpr int kButtonUpPin = 26;
constexpr int kButtonDownPin = 14;
constexpr int kButtonSelectPin = 27;
constexpr int kI2cSclPin = 22;
constexpr int kI2cSdaPin = 21;

// Backward-compat macros (used by libraries and legacy code)
#define LEDC_CHANNEL kLedcChannel
#define LEDC_RESOLUTION kLedcResolution
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define MEDIAN_WND kMedianWindow
#define MEAN_WND kMeanWindow
#define BUZZER_PIN kBuzzerPin
#define RESET_PIN kResetPin
#define BATTERY_PIN kBatteryPin
#define LDR_PIN kLdrPin
#define BUTTON_UP_PIN kButtonUpPin
#define BUTTON_DOWN_PIN kButtonDownPin
#define BUTTON_SELECT_PIN kButtonSelectPin
#define I2C_SCL_PIN kI2cSclPin
#define I2C_SDA_PIN kI2cSdaPin

Adafruit_BME280 bme280;
Adafruit_BMP280 bmp280;
Adafruit_HTU21DF htu21df;
Adafruit_SHT31 sht31;

constexpr int kR2d2BaudRate = 100; // Per-character tone duration for r2d2() beeps (ms)

constexpr auto kUsedPhotocell = LightDependentResistor::GL5516;
constexpr int kPhotocellSeriesResistor = 10000;
#define USED_PHOTOCELL kUsedPhotocell
#define PHOTOCELL_SERIES_RESISTOR kPhotocellSeriesResistor

MelodyPlayer player(BUZZER_PIN, 1, LOW);

EasyButton button_left(BUTTON_UP_PIN);
EasyButton button_right(BUTTON_DOWN_PIN);
EasyButton button_select(BUTTON_SELECT_PIN);
EasyButton button_reset(RESET_PIN);

LightDependentResistor photocell(LDR_PIN,
                                 PHOTOCELL_SERIES_RESISTOR,
                                 USED_PHOTOCELL,
                                 10,
                                 10);

int readIndex = 0;
int sampleIndex = 0;
unsigned long previousMillis_BatTempHum = 0;
unsigned long previousMillis_LDR = 0;
const unsigned long interval_BatTempHum = 10000;
const unsigned long interval_LDR = 100;
int total = 0;
unsigned long startTime;

MedianFilter<uint16_t> medianFilterBatt(MEDIAN_WND);
MedianFilter<uint16_t> medianFilterLDR(MEDIAN_WND);
MeanFilter<uint16_t> meanFilterBatt(MEAN_WND);
MeanFilter<uint16_t> meanFilterLDR(MEAN_WND);

float brightnessPercent = 0.0;

PeripheryManager_::PeripheryManager_()
{
    this->buttonL = &button_left;
    this->buttonR = &button_right;
    this->buttonS = &button_select;
    this->buttonRST = &button_reset;
}

// The getter for the instantiated singleton instance
PeripheryManager_& PeripheryManager_::getInstance()
{
    static PeripheryManager_ instance;
    return instance;
}

// Initialize the global shared instance
PeripheryManager_& PeripheryManager = PeripheryManager.getInstance();

void left_button_pressed()
{
    if (!appConfig.blockNavigation)
    {
        PeripheryManager.dispatchLeftButton();
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Left button clicked"));
    }
    else
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Left button clicked but blocked"));
    }
}

void right_button_pressed()
{
    if (!appConfig.blockNavigation)
    {
        PeripheryManager.dispatchRightButton();
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Right button clicked"));
    }
    else
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Right button clicked but blocked"));
    }
}

void select_button_pressed()
{
    if (!appConfig.blockNavigation)
    {
        PeripheryManager.dispatchSelectButton();
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Select button clicked"));
    }
    else
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Select button clicked but blocked"));
    }
}

void reset_button_pressed_long()
{
    PeripheryManager.dispatchFactoryReset();
    ESP.restart();
}

void select_button_pressed_long()
{
    if (systemConfig.apMode)
    {
        ++displayConfig.matrixLayout;
        if (displayConfig.matrixLayout < 0)
            displayConfig.matrixLayout = 2;
        saveSettings();
        ESP.restart();
    }
    else if (!appConfig.blockNavigation)
    {
        PeripheryManager.dispatchSelectButtonLong();
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Select button pressed long"));
    }
}

void select_button_double()
{
    if (systemConfig.debugMode)
        DEBUG_PRINTLN(F("Select button double pressed"));
    if (!appConfig.blockNavigation)
    {
        PeripheryManager.dispatchPowerToggle(displayConfig.matrixOff);
    }
}

void PeripheryManager_::playBootSound()
{
    if (systemConfig.debugMode)
        DEBUG_PRINTLN(F("Playing bootsound"));
    if (!audioConfig.soundActive)
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Sound output disabled"));
        return;
    }

    if (audioConfig.bootSound == "")
    {
        const int nNotes = 6;
        String notes[nNotes] = {"E5", "C5", "G4", "E4", "G4", "C5"};
        const int timeUnit = 150;
        Melody melody = MelodyFactory.load("Bootsound", timeUnit, notes, nNotes);
        player.playAsync(melody);
    }
    else
    {
        playFromFile(audioConfig.bootSound);
    }
}

void PeripheryManager_::stopSound()
{
    player.stop();
}

void PeripheryManager_::setVolume(uint8_t vol)
{
    player.setVolume(scaleVolume(vol, 30, 255));
}

bool PeripheryManager_::parseSound(const char *json)
{
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        return playFromFile(String(json));
    }
    if (doc.containsKey("sound"))
    {
        return playFromFile(doc["sound"].as<String>());
    }
    return false;
}

const char *PeripheryManager_::playRTTTLString(String rtttl)
{
    if (!audioConfig.soundActive)
        return nullptr;

    static char melodyName[64];
    Melody melody = MelodyFactory.loadRtttlString(rtttl.c_str());
    player.playAsync(melody);
    strncpy(melodyName, melody.getTitle().c_str(), sizeof(melodyName));
    melodyName[sizeof(melodyName) - 1] = '\0';
    return melodyName;
}

const char *PeripheryManager_::playFromFile(String file)
{
    if (!audioConfig.soundActive)
        return "";

    if (systemConfig.debugMode)
        DEBUG_PRINTLN(F("Playing RTTTL sound file"));
    if (LittleFS.exists("/MELODIES/" + file + ".txt"))
    {
        static char melodyName[64];
        Melody melody = MelodyFactory.loadRtttlFile("/MELODIES/" + file + ".txt");
        player.playAsync(melody);
        strncpy(melodyName, melody.getTitle().c_str(), sizeof(melodyName));
        melodyName[sizeof(melodyName) - 1] = '\0';
        return melodyName;
    }
    return NULL;
}

bool PeripheryManager_::isPlaying()
{
    return player.isPlaying();
}

void PeripheryManager_::setup()
{
    if (systemConfig.debugMode)
        DEBUG_PRINTLN(F("Setup periphery"));
    startTime = millis();
    pinMode(LDR_PIN, INPUT);
    pinMode(RESET_PIN, INPUT);
    button_left.begin();
    button_right.begin();
    button_select.begin();
    button_reset.begin();

    if ((displayConfig.rotateScreen && !systemConfig.swapButtons) || (!displayConfig.rotateScreen && systemConfig.swapButtons))
    {
        Serial.println("Button rotation");
        button_left.onPressed(right_button_pressed);
        button_right.onPressed(left_button_pressed);
    }
    else
    {
        button_left.onPressed(left_button_pressed);
        button_right.onPressed(right_button_pressed);
    }

    button_select.onPressed(select_button_pressed);
    button_select.onPressedFor(1000, select_button_pressed_long);
    button_select.onSequence(2, 300, select_button_double);

#ifdef ULANZI
    button_reset.onPressedFor(5000, reset_button_pressed_long);
#endif

    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

    if (bme280.begin(BME280_ADDRESS) || bme280.begin(BME280_ADDRESS_ALTERNATE))
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("BME280 sensor detected"));
        sensorConfig.tempSensorType = TEMP_SENSOR_TYPE_BME280;
    }
    else if (bmp280.begin(BMP280_ADDRESS) || bmp280.begin(BMP280_ADDRESS_ALT))
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("BMP280 sensor detected"));
        sensorConfig.tempSensorType = TEMP_SENSOR_TYPE_BMP280;
    }
    else if (htu21df.begin())
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("HTU21DF sensor detected"));
        sensorConfig.tempSensorType = TEMP_SENSOR_TYPE_HTU21DF;
    }
    else if (sht31.begin(0x44))
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("SHT31 sensor detected"));
        sensorConfig.tempSensorType = TEMP_SENSOR_TYPE_SHT31;
    }

    if (!brightnessConfig.ldrOnGround)
        photocell.setPhotocellPositionOnGround(false);
}

void PeripheryManager_::tick()
{
    bool menuActive = isMenuActive_ ? isMenuActive_() : false;
    if (!menuActive)
    {
        if (displayConfig.rotateScreen)
        {
            dispatchButtonReport(2, button_left.read());
            dispatchButtonReport(0, button_right.read());
        }
        else
        {
            dispatchButtonReport(0, button_left.read());
            dispatchButtonReport(2, button_right.read());
        }

        dispatchButtonReport(1, button_select.read());
    }
    else
    {
        button_left.read();
        button_select.read();
        button_right.read();
    }

    button_reset.read();

    unsigned long currentMillis_BatTempHum = millis();
    if (currentMillis_BatTempHum - previousMillis_BatTempHum >= interval_BatTempHum)
    {
        previousMillis_BatTempHum = currentMillis_BatTempHum;
        uint16_t ADCVALUE = analogRead(BATTERY_PIN);
        // Discard values that are totally out of range, especially the first value read after a reboot.
        // Meaningful values for an Ulanzi clock are in the range 400..700
        if ((ADCVALUE > 100) && (ADCVALUE < 1000))
        {
            // Send ADC values through median filter to get rid of the remaining spikes and then calculate the average
            batteryConfig.raw = meanFilterBatt.AddValue(medianFilterBatt.AddValue(ADCVALUE));
            batteryConfig.percent = calculateBatteryPercent(batteryConfig.raw, batteryConfig.minRaw, batteryConfig.maxRaw);
            sensorConfig.sensorsStable = true;
        }
        if (sensorConfig.sensorReading)
        {
            switch (sensorConfig.tempSensorType)
            {
            case TEMP_SENSOR_TYPE_BME280:
                sensorConfig.currentTemp = bme280.readTemperature();
                sensorConfig.currentHum = bme280.readHumidity();
                break;
            case TEMP_SENSOR_TYPE_BMP280:
                sensorConfig.currentTemp = bmp280.readTemperature();
                sensorConfig.currentHum = 0;
                break;
            case TEMP_SENSOR_TYPE_HTU21DF:
                sensorConfig.currentTemp = htu21df.readTemperature();
                sensorConfig.currentHum = htu21df.readHumidity();
                break;
            case TEMP_SENSOR_TYPE_SHT31:
                sht31.readBoth(&sensorConfig.currentTemp, &sensorConfig.currentHum);
                break;
            default:
                sensorConfig.currentTemp = 0;
                sensorConfig.currentHum = 0;
                break;
            }

            sensorConfig.currentTemp = applySensorOffset(sensorConfig.currentTemp, sensorConfig.tempOffset);
            sensorConfig.currentHum = applySensorOffset(sensorConfig.currentHum, sensorConfig.humOffset);
        }
        else
        {
            sensorConfig.sensorsStable = true;
        }
    }

    unsigned long currentMillis_LDR = millis();
    if (currentMillis_LDR - previousMillis_LDR >= interval_LDR)
    {
        previousMillis_LDR = currentMillis_LDR;

        uint16_t LDRVALUE = analogRead(LDR_PIN);
        LDRVALUE = applyLdrInversion(LDRVALUE, brightnessConfig.ldrOnGround);
        // Send LDR values through median filter to get rid of the remaining spikes and then calculate the average
        sensorConfig.ldrRaw = meanFilterLDR.AddValue(medianFilterLDR.AddValue(LDRVALUE));
        sensorConfig.currentLux = (roundf(photocell.getSmoothedLux() * 1000) / 1000);
        if (brightnessConfig.autoBrightness && !displayConfig.matrixOff)
        {
            brightnessConfig.brightness = calculateBrightness(sensorConfig.ldrRaw, brightnessConfig.ldrFactor, brightnessConfig.ldrGamma, brightnessConfig.minBrightness, brightnessConfig.maxBrightness);
            if (onBrightnessChange_)
                onBrightnessChange_(brightnessConfig.brightness);
        }
    }
}

unsigned long long PeripheryManager_::readUptime()
{
    static unsigned long lastTime = 0;
    static unsigned long long totalElapsed = 0;

    unsigned long currentTime = millis();
    if (currentTime < lastTime)
    {
        // millis() overflow
        totalElapsed += 4294967295UL - lastTime + currentTime + 1;
    }
    else
    {
        totalElapsed += currentTime - lastTime;
    }
    lastTime = currentTime;

    unsigned long long uptimeSeconds = totalElapsed / 1000;
    return uptimeSeconds;
}

void PeripheryManager_::r2d2(const char *msg)
{
#ifdef ULANZI
    for (int i = 0; msg[i] != '\0'; i++)
    {
        char c = msg[i];
        tone(BUZZER_PIN, (c - 'A' + 1) * 50);
        delay(kR2d2BaudRate + 10);
    }
    noTone(BUZZER_PIN);
#endif
}

// --- Interface registration ---

void PeripheryManager_::addButtonHandler(IButtonHandler *handler)
{
    buttonHandlers_.push_back(handler);
}

void PeripheryManager_::addButtonReporter(IButtonReporter *reporter)
{
    buttonReporters_.push_back(reporter);
}

void PeripheryManager_::setOnPowerToggle(void (*cb)(bool))
{
    onPowerToggle_ = cb;
}

void PeripheryManager_::setOnBrightnessChange(void (*cb)(int))
{
    onBrightnessChange_ = cb;
}

void PeripheryManager_::setOnFactoryReset(void (*cb)())
{
    onFactoryReset_ = cb;
}

void PeripheryManager_::setIsMenuActive(bool (*cb)())
{
    isMenuActive_ = cb;
}

// --- Button dispatch ---

void PeripheryManager_::dispatchLeftButton()
{
    for (auto *h : buttonHandlers_)
        h->leftButton();
}

void PeripheryManager_::dispatchRightButton()
{
    for (auto *h : buttonHandlers_)
        h->rightButton();
}

void PeripheryManager_::dispatchSelectButton()
{
    for (auto *h : buttonHandlers_)
        h->selectButton();
}

void PeripheryManager_::dispatchSelectButtonLong()
{
    for (auto *h : buttonHandlers_)
        h->selectButtonLong();
}

void PeripheryManager_::dispatchPowerToggle(bool state)
{
    if (onPowerToggle_)
        onPowerToggle_(state);
}

void PeripheryManager_::dispatchFactoryReset()
{
    if (onFactoryReset_)
        onFactoryReset_();
}

void PeripheryManager_::dispatchButtonReport(byte btn, bool state)
{
    for (auto *r : buttonReporters_)
        r->sendButton(btn, state);
}