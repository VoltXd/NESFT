#include "NES/APUFrameCounter.hpp"

void APUFrameCounter::reset()
{
	mApuClockDivider.loadPeriod(1);
	mApuClockDivider.reloadCounter();

	mIs5StepsMode = false;
	mIsInterruptInhibited = false;
	mIsIRQSignalSet = false;
	mIsTimerReset = false;
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
			mCycleCount++;

		fcState = APUFrameCounterState::IDLE;
	}
	else
	{
		switch (mCycleCount)
		{
			case 0:
				if (mIsTimerReset)
				{
					mIsTimerReset = false;
					fcState = APUFrameCounterState::HALF;
				}
				else
					fcState = APUFrameCounterState::IDLE;
				break;

			case FC_STEP1_CYCLE_COUNT:
			case FC_STEP3_CYCLE_COUNT:
				fcState = APUFrameCounterState::QUARTER;
				break;
				
			case FC_STEP5_CYCLE_COUNT:
				mIsEndReached = true;
			case FC_STEP2_CYCLE_COUNT:
				fcState = APUFrameCounterState::HALF;
				break;
			
			case FC_STEP4_CYCLE_COUNT:
				if (mIs5StepsMode)
					fcState = APUFrameCounterState::IDLE;
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
	}

	return fcState;
}

void APUFrameCounter::writeRegister(u8 reg)
{
	mIs5StepsMode = (reg & 0b1000'0000) != 0;
	mIsInterruptInhibited = (reg & 0b0100'0000) != 0;

	mCycleCount = (mApuClockDivider.getCounter() == 0) ? -3 : -4;
	mIsTimerReset = true;
}
