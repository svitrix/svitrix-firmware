#pragma once

#include <Arduino.h>
#include "IUpdater.h"

class IDisplayRenderer;

class UpdateManager_ : public IUpdater
{
  private:
    UpdateManager_() = default;
    UpdateManager_(const UpdateManager_&) = delete;
    UpdateManager_& operator=(const UpdateManager_&) = delete;
    UpdateManager_(UpdateManager_&&) = delete;
    UpdateManager_& operator=(UpdateManager_&&) = delete;

  public:
    static UpdateManager_& getInstance();
    void setDisplay(IDisplayRenderer *d);
    bool hasDisplay() const;
    void setup();
    bool checkUpdate(bool) override;
    void updateFirmware() override;
};

extern UpdateManager_& UpdateManager;
