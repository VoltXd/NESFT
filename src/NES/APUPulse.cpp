#include "NES/APUPulse.hpp"

APUPulse::APUPulse(bool isPulse1)
{
	mIsPulse1 = isPulse1;
}

void APUPulse::reset()
{
	mEnvelope.reset();
	mTimer.reset();
	mLengthCounter.reset();
	mSweep.reset();
	
	mDutyCycle = 0;
	mSequenceIndex = 0;
	mOutput = 0;
}

void APUPulse::update(APUFrameCounterState fcState)
{
	// Envelope
	if (fcState == APUFrameCounterState::QUARTER ||
	    fcState == APUFrameCounterState::HALF)
		mEnvelope.onClock();

	// Timer -> Sequencer
	// TODO: Timer/2 https://www.slack.net/~ant/nes-emu/apu_ref.txt
	u8 sequenceValue = SEQUENCER_LUT[mDutyCycle][mSequenceIndex];
	bool isTimerClocking = mTimer.countDown();
	if (isTimerClocking)
		incrementSequenceIndex();

	// Sweep period values
	u16 timerPeriod = mTimer.getPeriod();
	u16 targetPeriod = mIsPulse1 ?
					   mSweep.calculateTargetPeriod1(timerPeriod) :
					   mSweep.calculateTargetPeriod2(timerPeriod);

	if (fcState == APUFrameCounterState::HALF)
	{
		// Sweep -> Timer
		bool needsToUpdateTimer = mSweep.update(timerPeriod, targetPeriod);	
		if (needsToUpdateTimer)
			mTimer.loadPeriod(targetPeriod);

		// Length counter
		mLengthCounter.update();
	}
	u8 lengthCount = mLengthCounter.getCounter();
	
	// Gates (Sweep, sequencer & length counter)
	if (mSweep.isMuting(timerPeriod, targetPeriod) || 
	    (sequenceValue == 0)                       || 
		(lengthCount == 0))
		mOutput = 0;
	else
		mOutput = mEnvelope.getOutput();
}

void APUPulse::setReg0(u8 value)
{
	// reg0: DDlc vvvv
	mDutyCycle = (value & 0b1100'0000) >> 6; 
	bool isLoopFlagSet = (value & 0b0010'0000) != 0;
	bool isConstantVolume = (value & 0b0001'0000) != 0;
	u8 volume = (value & 0b0000'1111);

	// Set envelope flags
	mEnvelope.setLoopFlag(isLoopFlagSet);
	mEnvelope.setConstantVolumeFlag(isConstantVolume);
	mEnvelope.setVolume(volume);

	// Set length counter flag
	mLengthCounter.setHaltFlag(isLoopFlagSet);
}

void APUPulse::setReg1(u8 value)
{
	// reg1: EPPP NSSS
	bool isSweepEnabled = (value & 0b1000'0000) != 0;
	u8 dividerPeriod = (value & 0b0111'0000) >> 4;
	bool isNegateEnabled = (value & 0b0000'1000) != 0;
	u8 shiftCount = value & 0b0000'0111;

	// Set sweep parameters
	mSweep.setEnableFlag(isSweepEnabled);
	mSweep.setDividerPeriod(dividerPeriod);
	mSweep.setNegateFlag(isNegateEnabled);
	mSweep.setShiftCount(shiftCount);

	// Side effect
	mSweep.setReloadFlag();
}

void APUPulse::setReg2(u8 value)
{
	// reg2: LLLL LLLL
	u16 previousPeriod = mTimer.getPeriod();
	u16 newPeriod = (previousPeriod & 0b0111'0000'0000) | value;
	mTimer.loadPeriod(newPeriod);
}

void APUPulse::setReg3(u8 value)
{
	// reg3: llll lHHH
	u8 lengthCounterIdx = (value & 0b1111'1000) >> 3;
	u16 timerPeriodMsb = value & 0b0000'0111;

	// Set lengthCounterCount
	mLengthCounter.setCounter(lengthCounterIdx);

	// Set Envelope start flag
	mEnvelope.setStartFlag();

	// Set timer period
	u16 previousPeriod = mTimer.getPeriod();
	u16 newPeriod = (previousPeriod & 0b0000'1111'1111) | (timerPeriodMsb << 8);
	mTimer.loadPeriod(newPeriod);

	// Restart sequencer
	mSequenceIndex = 0;
}
