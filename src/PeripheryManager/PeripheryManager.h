#pragma once
#include <Arduino.h>
#include <EasyButton.h>
#include <vector>
#include "IPeripheryProvider.h"
#include "ISound.h"

class IButtonHandler;
class IButtonReporter;

class PeripheryManager_ : public IPeripheryProvider, public ISound
{
  private:
    PeripheryManager_();
    PeripheryManager_(const PeripheryManager_&) = delete;
    PeripheryManager_& operator=(const PeripheryManager_&) = delete;
    PeripheryManager_(PeripheryManager_&&) = delete;
    PeripheryManager_& operator=(PeripheryManager_&&) = delete;
#ifdef ULANZI
    const int BatReadings = 10;
    uint16_t TotalBatReadings[10];
#endif
    int readIndex = 0;
    uint16_t total = 0;
    uint16_t average = 0;
    const int LDRReadings = 30;
    uint16_t TotalLDRReadings[30];
    int sampleIndex = 0;
    unsigned long previousMillis = 0;
    const unsigned long interval = 1000;

    std::vector<IButtonHandler *> buttonHandlers_;
    std::vector<IButtonReporter *> buttonReporters_;
    void (*onPowerToggle_)(bool) = nullptr;
    void (*onBrightnessChange_)(int) = nullptr;
    void (*onFactoryReset_)() = nullptr;
    bool (*isMenuActive_)() = nullptr;

  public:
    EasyButton *buttonL;
    EasyButton *buttonR;
    EasyButton *buttonS;
    EasyButton *buttonRST;

    static PeripheryManager_& getInstance();
    void setup();
    void tick();
    void playBootSound();
    const char *playFromFile(String file);
    const char *playRTTTLString(String rtttl) override;
    bool parseSound(const char *json) override;
    bool isPlaying();
    void stopSound() override;
    void r2d2(const char *msg) override;
    void setVolume(uint8_t) override;
    unsigned long long readUptime() override;

    // Interface registration
    void addButtonHandler(IButtonHandler *handler);
    void addButtonReporter(IButtonReporter *reporter);
    void setOnPowerToggle(void (*cb)(bool));
    void setOnBrightnessChange(void (*cb)(int));
    void setOnFactoryReset(void (*cb)());
    void setIsMenuActive(bool (*cb)());

    // Button dispatch (called by button callbacks)
    void dispatchLeftButton();
    void dispatchRightButton();
    void dispatchSelectButton();
    void dispatchSelectButtonLong();
    void dispatchPowerToggle(bool state);
    void dispatchFactoryReset();
    void dispatchButtonReport(byte btn, bool state);
};

extern PeripheryManager_& PeripheryManager;
