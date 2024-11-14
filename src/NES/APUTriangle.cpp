#include "NES/APUTriangle.hpp"

void APUTriangle::reset()
{
	mTimer.reset();
	mLengthCounter.reset();
	mLinearCounterValue = 0;
	mOutput = 0;
}

void APUTriangle::update(APUFrameCounterState fcState)
{
	// TODO: mute if timer period < 2 (ultra sound)
	// Timer
	bool isTimerClocking = mTimer.countDown();
	bool isUltraSonic = mTimer.getPeriod() < 2;
	
	// Linear counter
	if (fcState == APUFrameCounterState::HALF ||
	    fcState == APUFrameCounterState::QUARTER)
	{
		if (mIsCounterReloadFlagSet)
			mLinearCounterValue = mLinCounterReloadValue;
		else if (mLinearCounterValue != 0)
			mLinearCounterValue--;
		
		if (!mIsControlFlagSet)
			mIsCounterReloadFlagSet = false;
	}
	u8 linearCount = mLinearCounterValue;

	// Length counter
	if (fcState == APUFrameCounterState::HALF)
	{
		mLengthCounter.update();
	}
	u8 lengthCount = mLengthCounter.getCounter();

	if (isTimerClocking  &&
		!isUltraSonic    &&
	    linearCount != 0 &&
		lengthCount != 0)
		incrementSequenceIndex();
	
	mOutput = SEQUENCER_LUT[mSequenceIndex];
}

void APUTriangle::setReg0(u8 value)
{
	// reg0: CRRR RRRR
	mIsControlFlagSet = (value & 0b1000'0000) != 0;
	mLinCounterReloadValue = value & 0b0111'1111;

	mLengthCounter.setHaltFlag(mIsControlFlagSet);
}

void APUTriangle::setReg1(u8 value)
{
	// reg1: LLLL LLLL
	u16 previousPeriod = mTimer.getPeriod();
	u16 newPeriod = (previousPeriod & 0b0111'0000'0000) | value;
	mTimer.loadPeriod(newPeriod);
}

void APUTriangle::setReg2(u8 value)
{
	// reg2: llll lHHH
	u8 lengthCounterIdx = (value & 0b1111'1000) >> 3;
	u16 timerPeriodMsb = value & 0b0000'0111;

	// Set lengthCounterCount
	mLengthCounter.setCounter(lengthCounterIdx);

	// Set linear counter reload flag
	mIsCounterReloadFlagSet = true;
	
	// Set timer period
	u16 previousPeriod = mTimer.getPeriod();
	u16 newPeriod = (previousPeriod & 0b0000'1111'1111) | (timerPeriodMsb << 8);
	mTimer.loadPeriod(newPeriod);
}
