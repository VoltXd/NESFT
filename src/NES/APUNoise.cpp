#include "NES/APUNoise.hpp"

void APUNoise::reset()
{
	mEnvelope.reset();
	mLengthCounter.reset();
	mTimer.reset();

	mIsModeFlagSet = false;
	mShiftRegister = 1;
	mOutput = 0;
}

void APUNoise::update(APUFrameCounterState fcState)
{
	// Envelope
	if (fcState == APUFrameCounterState::QUARTER ||
	    fcState == APUFrameCounterState::HALF)
		mEnvelope.onClock();

	// Timer -> Shift register
	bool isTimerClocking = mTimer.countDown();
	if (isTimerClocking)
	{
		// Shift & feedback
		u8 feedbackBit = mIsModeFlagSet ? 6 : 1;
		u16 msb = (mShiftRegister & 1) ^ 
		          ((mShiftRegister & (1 << feedbackBit)) >> feedbackBit);
		mShiftRegister |= msb << 15; // Feedback
		mShiftRegister >>= 1;        // Shift
	}

	// Length counter
	if (fcState == APUFrameCounterState::HALF)
		mLengthCounter.update();
	u8 lengthCount = mLengthCounter.getCounter();
	
	// Gates (shift reg  & length counter)
	if (((mShiftRegister & 1) != 0) || (lengthCount == 0))
		mOutput = 0;
	else
		mOutput = mEnvelope.getOutput();
}

void APUNoise::setReg0(u8 value)
{
	// reg0: --lc vvvv
	bool isLoopFlagSet = (value & 0b0010'0000) != 0;
	bool isConstantVolume = (value & 0b0001'0000) != 0;
	u8 volume = (value & 0b0000'1111);

	// Set 3 flags for envelope
	mEnvelope.setLoopFlag(isLoopFlagSet);
	mEnvelope.setConstantVolumeFlag(isConstantVolume);
	mEnvelope.setVolume(volume);

	// Set 1 flag for length counter
	mLengthCounter.setHaltFlag(isLoopFlagSet);
}

void APUNoise::setReg1(u8 value)
{
	// reg1: M--- PPPP
	mIsModeFlagSet = (value & 0b1000'0000) != 0;
	u8 periodIdx = value & 0b0000'1111;

	// Set timer period (IN APU CYCLE !!!!!)
	mTimer.loadPeriod(TIMER_NTSC_PERIOD_LUT[periodIdx]);
}

void APUNoise::setReg2(u8 value)
{
	// reg1: llll l---
	u8 lengthCounterIdx = (value & 0b1111'1000) >> 3;

	// Set lengthCounterCount
	mLengthCounter.setCounter(lengthCounterIdx);

	// Set Envelope start flag
	mEnvelope.setStartFlag();
}
