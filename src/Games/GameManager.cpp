#include "GameManager.h"
#include "IDisplayRenderer.h"
#include "Globals.h"
#include "ServerManager.h"
#include <ArduinoJson.h>
#include <cassert>

void GameManager_::setDisplay(IDisplayRenderer *d)
{
    assert(d);
    display_ = d;
}
bool GameManager_::hasDisplay() const
{
    return display_ != nullptr;
}

GameManager_& GameManager_::getInstance()
{
    static GameManager_ instance;
    return instance;
}

GameManager_& GameManager = GameManager.getInstance();

void GameManager_::setup()
{
    switch (currentState)
    {
    case Slot_Machine:
        slotMachine.setup();
        break;
    case SVITRIX_Says:
        SvitrixSays.setup();
        break;
    default:
        break;
    }
}

void GameManager_::tick()
{
    switch (currentState)
    {
    case None:
        display_->drawFilledRect(0, 0, 32, 8, 0);
        display_->printText(0, 6, "SELECT", true, 0);
        break;
    case Slot_Machine:
        slotMachine.tick();
        break;
    case SVITRIX_Says:
        SvitrixSays.tick();
        break;
    default:
        break;
    }
}

void GameManager_::selectPressed()
{
    switch (currentState)
    {
    case Slot_Machine:
        slotMachine.selectPressed();
        break;
    case SVITRIX_Says:
        SvitrixSays.selectPressed();
        break;
    default:
        break;
    }
}

void GameManager_::ControllerInput(const char *cmd)
{
    switch (currentState)
    {
    case Slot_Machine:
        slotMachine.ControllerInput(cmd);
        break;
    case SVITRIX_Says:
        SvitrixSays.ControllerInput(cmd);
        break;
    default:
        break;
    }
}

bool GameManager_::isActive() const
{
    return gameActive_;
}

void GameManager_::start(bool active)
{
    currentState = None;
    gameActive_ = active;
}

void GameManager_::ChooseGame(short game)
{
    sendPoints(0);
    switch (game)
    {
    case 0:
        currentState = Slot_Machine;
        setup();
        break;
    case 1:
        currentState = SVITRIX_Says;
        setup();
        break;
    default:
        break;
    }
}

void GameManager_::sendPoints(int points)
{
    DynamicJsonDocument doc(1024);
    doc["points"] = points;
    String jsonString;
    serializeJson(doc, jsonString);
    ServerManager.sendTCP(jsonString);
}