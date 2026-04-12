#pragma once

#include <Arduino.h>
#include "IButtonHandler.h"

class IDisplayRenderer;
class IDisplayControl;
class IDisplayNavigation;
class IPeripheryProvider;
class IUpdater;

class MenuManager_ : public IButtonHandler
{
  private:
    MenuManager_() = default;
    MenuManager_(const MenuManager_&) = delete;
    MenuManager_& operator=(const MenuManager_&) = delete;
    MenuManager_(MenuManager_&&) = delete;
    MenuManager_& operator=(MenuManager_&&) = delete;
    IDisplayRenderer *renderer_ = nullptr;
    IDisplayControl *control_ = nullptr;
    IDisplayNavigation *nav_ = nullptr;
    IPeripheryProvider *periphery_ = nullptr;
    IUpdater *updater_ = nullptr;

  public:
    static MenuManager_& getInstance();
    void setDisplay(IDisplayRenderer *r, IDisplayControl *c, IDisplayNavigation *n);
    bool hasDisplay() const
    {
        return renderer_ && control_ && nav_;
    }
    void setServices(IPeripheryProvider *pp, IUpdater *u);
    bool hasServices() const
    {
        return periphery_ && updater_;
    }
    bool inMenu = false;
    String menutext();
    void rightButton() override;
    void leftButton() override;
    void selectButton() override;
    void selectButtonLong() override;
};

extern MenuManager_& MenuManager;
