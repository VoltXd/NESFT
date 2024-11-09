#pragma once

#include "NES/Config.hpp"

enum ControllerInput
{
    A      = 1 << 0,
    B      = 1 << 1,
    SELECT = 1 << 2,
    START  = 1 << 3,
    UP     = 1 << 4,
    DOWN   = 1 << 5,
    LEFT   = 1 << 6,
    RIGHT  = 1 << 7
};

class Controller
{
public:
    u8 getStateBitAndShift();
    void setStrobe(u8 value);
    void updateControllerState(ControllerInput input, bool isPressed);

private:
    u8 mControllerState         = 0;
    u8 mControllerShiftRegister = 0;

    bool mIsUpdatingState       = true;
};