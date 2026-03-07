#pragma once

#include <Arduino.h>

enum SvitrixSaysState
{
    SVITRIX_READY,
    SVITRIX_SHOWSEQ,
    SVITRIX_USERINPUT,
    SVITRIX_PAUSE,
    SVITRIX_LOSE,
    SVITRIX_WIN
};

class IDisplayRenderer;

class SvitrixSays_
{
  private:
    SvitrixSays_() = default;
    SvitrixSays_(const SvitrixSays_&) = delete;
    SvitrixSays_& operator=(const SvitrixSays_&) = delete;
    SvitrixSays_(SvitrixSays_&&) = delete;
    SvitrixSays_& operator=(SvitrixSays_&&) = delete;
    IDisplayRenderer *display_ = nullptr;
    SvitrixSaysState currentState = SVITRIX_READY;
    static const int MAX_SEQ = 32;
    int sequence[MAX_SEQ];
    int sequenceLength = 0;
    int currentStep = 0;
    bool showing = false;
    unsigned long nextBlink = 0;
    int blinkIndex = 0;
    unsigned long loseStart = 0;
    unsigned long nextSequenceTime = 0;
    bool highlightUserInput = false;
    unsigned long highlightUserInputEnd = 0;
    int lastUserButton = -1;


    void generateSequence();
    void showSequence();
    void checkUserInput(int button);
    void drawSquares(bool highlight = false, int highlightIndex = -1);

  public:
    static SvitrixSays_& getInstance();
    void setDisplay(IDisplayRenderer *d);
    bool hasDisplay() const;
    void setup();
    void tick();
    void selectPressed();
    void ControllerInput(const char *cmd);
};

extern SvitrixSays_& SvitrixSays;
