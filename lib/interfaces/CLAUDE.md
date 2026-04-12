# Interfaces — AI Reference

13 pure virtual interfaces that decouple all module-to-module communication. This is the architectural backbone of Svitrix.

## Interface Map

| Interface | Methods | Implementor(s) | Consumers |
|-----------|---------|-----------------|-----------|
| **IDisplayRenderer** | 11 | DisplayRenderer_ | UpdateManager, MenuManager, ServerManager |
| **IDisplayControl** | 10 | DisplayManager_ | MenuManager, ServerManager, MQTTManager |
| **IDisplayNavigation** | 12 | DisplayManager_ | MenuManager, ServerManager, MQTTManager, DataFetcher |
| **IDisplayNotifier** | 9 | NotificationManager_ | ServerManager, MQTTManager |
| **IMatrixHost** | 4 | DisplayManager_ | MatrixDisplayUi |
| **IButtonHandler** | 4 | DisplayManager_, MenuManager_ | PeripheryManager |
| **IButtonReporter** | 1 | MQTTManager_, ServerManager_ | PeripheryManager |
| **INotifier** | 6 | MQTTManager_ | DisplayManager, NotificationManager |
| **IPeripheryProvider** | 3 | PeripheryManager_ | DisplayManager, NotificationManager, MenuManager, ServerManager, MQTTManager |
| **IPixelCanvas** | 6 | NeoMatrixCanvas | Effect system |
| **ISound** | 3 | PeripheryManager_ | ServerManager, MQTTManager |
| **IPower** | 2 | PowerManager_ | ServerManager, MQTTManager |
| **IUpdater** | 2 | UpdateManager_ | ServerManager, MQTTManager, MenuManager |

## Injection Pattern

All modules use **setter injection with assert guards**:

```cpp
// In consumer header:
class MenuManager_ {
    IDisplayRenderer *renderer_ = nullptr;
    IDisplayControl *control_ = nullptr;
public:
    void setDisplay(IDisplayRenderer *r, IDisplayControl *c, IDisplayNavigation *n);
    bool hasDisplay() const { return renderer_ && control_ && nav_; }
};

// In main.cpp:
MenuManager.setDisplay(&DisplayManager.getRenderer(), &DisplayManager, &DisplayManager);
assert(MenuManager.hasDisplay());
```

## Interface Details

### IDisplayRenderer (11 methods)
```cpp
void clear(), show();
void resetTextColor(), setTextColor(uint32_t);
void printText(x, y, text, centered, textCase);
void HSVtext(x, y, text, clear, textCase);
void drawFilledRect(x, y, w, h, color);
void drawRGBBitmap(x, y, bitmap, w, h);
void drawBMP(x, y, bitmap, w, h);
void drawProgressBar(x, y, progress, pColor, pbColor);
void drawMenuIndicator(cur, total, color);
```

### IDisplayControl (10 methods)
```cpp
void setBrightness(int), setPower(bool), powerStateParse(const char*);
bool setAutoTransition(bool);
void applyAllSettings(), setNewSettings(const char*);
String getSettings(), getStats();
bool moodlight(const char*);
String ledsAsJson();
```

### IDisplayNavigation (12 methods)
```cpp
void nextApp(), previousApp();
bool switchToApp(const char*);
void updateAppVector(const char*), reorderApps(const String&);
String getAppsAsJson(), getAppsWithIcon();
bool parseCustomPage(const String&, const char*, bool);
String getEffectNames(), getTransitionNames();
void loadNativeApps(), setCustomAppColors(uint32_t);
```

### IDisplayNotifier (9 methods)
```cpp
bool generateNotification(uint8_t source, const char* json);
void dismissNotify();
bool indicatorParser(uint8_t indicator, const char* json);
void setIndicator{1,2,3}Color(uint32_t);
void setIndicator{1,2,3}State(bool);
```

### IMatrixHost (4 methods)
```cpp
CRGB* getLeds();
void gammaCorrection();
void sendAppLoop();
bool setAutoTransition(bool);
```

### IButtonHandler (4 methods)
```cpp
void leftButton(), rightButton(), selectButton(), selectButtonLong();
```

### IButtonReporter (1 method)
```cpp
void sendButton(byte btn, bool state);  // btn: 0=left, 1=select, 2=right
```

### INotifier (6 methods)
```cpp
void publish(const char* topic, const char* payload);
void rawPublish(const char* prefix, const char* topic, const char* payload);
void setCurrentApp(String app);
void setIndicatorState(uint8_t indicator, bool state, uint32_t color);
bool subscribe(const char* topic);
long getReceivedMessages() const;
```

### IPeripheryProvider (3 methods)
```cpp
void stopSound();
void setVolume(uint8_t vol);
unsigned long long readUptime();
```

### IPixelCanvas (6 methods)
```cpp
void drawPixel(x, y, CRGB/CHSV/uint16_t color);
uint16_t Color(r, g, b);
void drawRGBBitmap(x, y, bitmap, w, h);
void fillRect(x, y, w, h, color);
```

### ISound (3 methods)
```cpp
const char* playRTTTLString(String rtttl);
bool parseSound(const char* json);
void r2d2(const char* msg);
```

### IPower (2 methods)
```cpp
void sleep(uint64_t seconds);
void sleepParser(const char* json);
```

### IUpdater (2 methods)
```cpp
bool checkUpdate(bool withScreen);
void updateFirmware();
```

## Known Violations (16 files bypass interfaces)

These files use direct `#include` instead of interfaces (see issue #11):

| File | Bypasses | Should Use |
|------|----------|------------|
| Overlays.cpp | DisplayManager, MenuManager, PeripheryManager, MQTTManager | Interfaces via injection |
| Apps_internal.h | DisplayManager | IDisplayRenderer |
| Apps_CustomApp.cpp | MQTTManager | INotifier |
| Apps_Helpers.cpp | MQTTManager | INotifier |
| AppContentRenderer.cpp | DisplayManager | IDisplayRenderer |
| MenuManager.cpp | ServerManager | New interface or callback |
| Globals.cpp | DisplayManager | Remove dependency |

## Missing Interfaces (Suggested)

| Interface | Why Needed |
|-----------|-----------|
| IServerConnectivity | MenuManager needs `isConnected()` — currently bypasses via direct include |

## Rules for Adding New Interfaces

1. Place in `lib/interfaces/src/`
2. Pure virtual only — no implementation, no state
3. Keep methods minimal — prefer small focused interfaces over large ones
4. Every consumer must use setter injection + assert guards
5. All wiring happens in `main.cpp` composition root
6. Update this table when adding
