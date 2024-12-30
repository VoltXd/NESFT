#include "NES/Controller.hpp"

#include <iostream>

u8 Controller::getStateBitAndShift()
{
    // Get button state and shift buttons register
    u8 buttonBit = mControllerShiftRegister & 0b0000'0001;

    if (mIsUpdatingState)
        mControllerShiftRegister = mControllerState;
    else
        mControllerShiftRegister = (mControllerShiftRegister >> 1) | 0b1000'0000;

    return buttonBit;
}

void Controller::setStrobe(u8 value)
{
    mIsUpdatingState = (value & 0b0000'0001) != 0;
    if (mIsUpdatingState)
        mControllerShiftRegister = mControllerState;
}

void Controller::updateControllerState(ControllerInput input, bool isPressed)
{
    if (isPressed)
        mControllerState |= input;
    else
        mControllerState &= ~input;

    if (mIsUpdatingState)
        mControllerShiftRegister = mControllerState;
}
