#pragma once

#include <Arduino.h>
#include "SlotMachine.h"
#include "SvitrixSays.h"

class IDisplayRenderer;

class GameManager_
{
    enum GameState
    {
        None,
        Slot_Machine,
        SVITRIX_Says
    };

  private:
    GameState currentState = None;
    bool gameActive_ = false;
    IDisplayRenderer *display_ = nullptr;
    SlotMachine_& slotMachine = SlotMachine_::getInstance();
    SvitrixSays_& SvitrixSays = SvitrixSays_::getInstance();
    GameManager_() = default;
    GameManager_(const GameManager_&) = delete;
    GameManager_& operator=(const GameManager_&) = delete;
    GameManager_(GameManager_&&) = delete;
    GameManager_& operator=(GameManager_&&) = delete;

  public:
    static GameManager_& getInstance();
    void setDisplay(IDisplayRenderer *d);
    bool hasDisplay() const;
    bool isActive() const;
    void setup();
    void tick();
    void start(bool active);
    void ControllerInput(const char *cmd);
    void selectPressed();
    void ChooseGame(short game);
    void sendPoints(int points);
};

extern GameManager_& GameManager;
