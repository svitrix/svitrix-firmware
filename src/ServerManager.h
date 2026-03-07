#pragma once

#include <Arduino.h>
#include "IButtonReporter.h"

class IDisplayRenderer;
class IDisplayControl;
class IDisplayNavigation;
class IDisplayNotifier;
class ISound;
class IPower;
class IUpdater;

class ServerManager_ : public IButtonReporter
{
  private:
    ServerManager_() = default;
    ServerManager_(const ServerManager_&) = delete;
    ServerManager_& operator=(const ServerManager_&) = delete;
    ServerManager_(ServerManager_&&) = delete;
    ServerManager_& operator=(ServerManager_&&) = delete;

  public:
    static ServerManager_& getInstance();
    void setDisplay(IDisplayRenderer *r, IDisplayControl *c, IDisplayNavigation *n, IDisplayNotifier *nt);
    bool hasDisplay() const;
    void setServices(ISound *s, IPower *p, IUpdater *u);
    bool hasServices() const;
    void setup();
    void tick();
    void loadSettings();
    void sendButton(byte btn, bool state) override;
    void erase();
    void sendTCP(String message);
    bool isConnected;
    IPAddress myIP;
};

extern ServerManager_& ServerManager;
