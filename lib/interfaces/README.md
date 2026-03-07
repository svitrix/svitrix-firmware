# Interfaces — AI Reference

> 13 pure virtual interfaces decoupling all module-to-module communication.
> All wiring in main.cpp (composition root). No consumer includes concrete manager headers.

## Interface Map

```
 Implementor              Interface              Consumers
─────────────────────────────────────────────────────────────
 DisplayRenderer_    ── IDisplayRenderer ──►  UpdateManager, GameManager,
                                              SlotMachine, SvitrixSays,
                                              MenuManager, ServerManager

 DisplayManager_     ── IDisplayControl ───►  MenuManager, ServerManager,
                                              MQTTManager

 DisplayManager_     ── IDisplayNavigation ►  MenuManager, ServerManager,
                                              MQTTManager

 NotificationManager_── IDisplayNotifier ──►  ServerManager, MQTTManager

 DisplayManager_     ── IMatrixHost ───────►  MatrixDisplayUi

 DisplayManager_     ┐
 MenuManager_        ├─ IButtonHandler ────►  PeripheryManager (dispatcher)

 MQTTManager_        ┐
 ServerManager_      ├─ IButtonReporter ──►  PeripheryManager (dispatcher)

 MQTTManager_        ── INotifier ─────────►  DisplayManager_

 PeripheryManager_   ── IPeripheryProvider ►  NotificationManager_, MQTTManager,
                                              MenuManager

 PeripheryManager_   ── ISound ────────────►  ServerManager, MQTTManager

 PowerManager_       ── IPower ────────────►  ServerManager, MQTTManager

 UpdateManager_      ── IUpdater ──────────►  ServerManager, MQTTManager,
                                              MenuManager
```

## IDisplayRenderer (11 methods)

```cpp
virtual void clear() = 0;
virtual void show() = 0;
virtual void resetTextColor() = 0;
virtual void setTextColor(uint32_t color) = 0;
virtual void printText(int16_t x, int16_t y, const char *text, bool centered, byte textCase) = 0;
virtual void HSVtext(int16_t x, int16_t y, const char *text, bool clear, byte textCase) = 0;
virtual void drawFilledRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color) = 0;
virtual void drawRGBBitmap(int16_t x, int16_t y, const uint32_t *bitmap, int16_t w, int16_t h) = 0;
virtual void drawBMP(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h) = 0;
virtual void drawProgressBar(int16_t x, int16_t y, int progress, uint32_t pColor, uint32_t pbColor) = 0;
virtual void drawMenuIndicator(int cur, int total, uint32_t color) = 0;
```

## IDisplayControl (10 methods)

```cpp
virtual void setBrightness(int bri) = 0;
virtual void setPower(bool state) = 0;
virtual void powerStateParse(const char *json) = 0;
virtual bool setAutoTransition(bool active) = 0;
virtual void applyAllSettings() = 0;
virtual void setNewSettings(const char *json) = 0;
virtual String getSettings() = 0;
virtual String getStats() = 0;
virtual bool moodlight(const char *json) = 0;
virtual String ledsAsJson() = 0;
```

## IDisplayNavigation (12 methods)

```cpp
virtual void nextApp() = 0;
virtual void previousApp() = 0;
virtual bool switchToApp(const char *json) = 0;
virtual void updateAppVector(const char *json) = 0;
virtual void reorderApps(const String &jsonString) = 0;
virtual String getAppsAsJson() = 0;
virtual String getAppsWithIcon() = 0;
virtual bool parseCustomPage(const String &name, const char *json, bool preventSave) = 0;
virtual String getEffectNames() = 0;
virtual String getTransitionNames() = 0;
virtual void loadNativeApps() = 0;
virtual void setCustomAppColors(uint32_t color) = 0;
```

## IDisplayNotifier (9 methods)

```cpp
virtual bool generateNotification(uint8_t source, const char *json) = 0;
virtual void dismissNotify() = 0;
virtual bool indicatorParser(uint8_t indicator, const char *json) = 0;
virtual void setIndicator1Color(uint32_t color) = 0;
virtual void setIndicator1State(bool state) = 0;
virtual void setIndicator2Color(uint32_t color) = 0;
virtual void setIndicator2State(bool state) = 0;
virtual void setIndicator3Color(uint32_t color) = 0;
virtual void setIndicator3State(bool state) = 0;
```

## IMatrixHost (4 methods)

```cpp
virtual CRGB *getLeds() = 0;
virtual void gammaCorrection() = 0;
virtual void sendAppLoop() = 0;
virtual bool setAutoTransition(bool active) = 0;
```

## IButtonHandler (4 methods)

```cpp
virtual void leftButton() = 0;
virtual void rightButton() = 0;
virtual void selectButton() = 0;
virtual void selectButtonLong() = 0;
```

## IButtonReporter (1 method)

```cpp
virtual void sendButton(byte btn, bool state) = 0;
// btn: 0=left, 1=select, 2=right; state: true=pressed
```

## INotifier (6 methods)

```cpp
virtual void publish(const char *topic, const char *payload) = 0;
virtual void rawPublish(const char *prefix, const char *topic, const char *payload) = 0;
virtual void setCurrentApp(String app) = 0;
virtual void setIndicatorState(uint8_t indicator, bool state, uint32_t color) = 0;
virtual bool subscribe(const char *topic) = 0;
virtual long getReceivedMessages() const = 0;
```

## IPeripheryProvider (3 methods)

```cpp
virtual void stopSound() = 0;
virtual void setVolume(uint8_t vol) = 0;
virtual unsigned long long readUptime() = 0;
```

## IPixelCanvas (6 methods)

```cpp
virtual void drawPixel(int16_t x, int16_t y, CRGB color) = 0;
virtual void drawPixel(int16_t x, int16_t y, CHSV color) = 0;
virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;
virtual uint16_t Color(uint8_t r, uint8_t g, uint8_t b) = 0;
virtual void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h) = 0;
virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) = 0;
```

## ISound (3 methods)

```cpp
virtual const char *playRTTTLString(String rtttl) = 0;
virtual bool parseSound(const char *json) = 0;
virtual void r2d2(const char *msg) = 0;
```

## IPower (2 methods)

```cpp
virtual void sleep(uint64_t seconds) = 0;
virtual void sleepParser(const char *json) = 0;
```

## IUpdater (2 methods)

```cpp
virtual bool checkUpdate(bool withScreen) = 0;
virtual void updateFirmware() = 0;
```
