#include "NES/APUFrameCounter.hpp"

void APUFrameCounter::reset()
{
	mApuClockDivider.loadPeriod(1);
	mApuClockDivider.reloadCounter();

	mIs5StepsMode = false;
	mIsRegBit7Set = false;
	mIsInterruptInhibited = false;
	mIsIRQSignalSet = false;
	mIsEndReached = false;
}

APUFrameCounterState APUFrameCounter::executeOneCpuCycle()
{
	APUFrameCounterState fcState;
	// Clock the divider
	mIsEvenCycle = mApuClockDivider.countDown();

	// On even cycles -> Frame counter progresses
	if (mIsEvenCycle)
	{
		if (mIsEndReached)
		{
			mIsEndReached = false;
			mCycleCount = 0;
		}
		else
		{
			mCycleCount++;
		}

		fcState = APUFrameCounterState::IDLE;
	}
	else
	{
		switch (mCycleCount)
		{
			case FC_STEP1_CYCLE_COUNT:
			case FC_STEP3_CYCLE_COUNT:
				fcState = APUFrameCounterState::QUARTER;
				break;
				
			case FC_STEP5_CYCLE_COUNT:
				mIsEndReached = true;
				[[fallthrough]];
			case FC_STEP2_CYCLE_COUNT:
				fcState = APUFrameCounterState::HALF;
				break;
			
			case FC_STEP4_CYCLE_COUNT:
				if (mIs5StepsMode)
				{
					fcState = APUFrameCounterState::IDLE;
				}
				else
				{
					fcState = APUFrameCounterState::HALF;
					if (!mIsInterruptInhibited)
						mIsIRQSignalSet = true;
					mIsEndReached = true;
				}
				break;
				
			default:
				fcState = APUFrameCounterState::IDLE;
				break;
		}

		// Quarter and Half signal are generated if 5 mode has been set through 0x4017
		if (mIsRegBit7Set)
		{
			mIsRegBit7Set = false;
			fcState = APUFrameCounterState::HALF;
		}
	}

	return fcState;
}

void APUFrameCounter::writeRegister(u8 reg)
{
	mIs5StepsMode = (reg & 0b1000'0000) != 0;
	mIsRegBit7Set = mIs5StepsMode;
	mIsInterruptInhibited = (reg & 0b0100'0000) != 0;

	// Clear IRQ on IRQ inhibited set
	if (mIsInterruptInhibited)
		mIsIRQSignalSet = false;

	if (mApuClockDivider.getCounter() == 0)
	{
		// 3 extra CPU cycles -> 1.5 APU Cycles
		mCycleCount = -1;
		mApuClockDivider.countDown();
	}
	else
	{
		// 4 extra CPU cycles -> 2 APU Cycles
		mCycleCount = -2;
	}
}
