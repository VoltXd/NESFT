#include "NES/APUSweep.hpp"

void APUSweep::reset()
{
	mIsEnabled = false;
	mDivider.reset();
	mIsNegating = false;
	mShiftCount = 0;
}

bool APUSweep::update(u16 timerPeriod, u16 targetPeriod)
{
	bool needsToUpdateTimer = false;
	if (mIsReloadFlagSet)
	{
		mDivider.reloadCounter();	
		mIsReloadFlagSet = false;
	}
	else
	{
		bool isDividerClocking = mDivider.countDown();
		if (isDividerClocking)
		{
			mIsReloadFlagSet = false;
			if (mIsEnabled && (mShiftCount != 0) && !isMuting(timerPeriod, targetPeriod))
				needsToUpdateTimer = true;
		}
	}

	return needsToUpdateTimer;
}

u16 APUSweep::calculateTargetPeriod1(u16 timerPeriod)
{
	u16 targetPeriod = timerPeriod;
	
	// Shift right
	u16 shiftedPeriod = timerPeriod >> mShiftCount;

	// Add/substract to current period 
	targetPeriod += !mIsNegating ? shiftedPeriod : -shiftedPeriod - 1;

	// Clamp to 0 if negative
	if ((targetPeriod & 0b1000'0000'0000'0000) != 0)
		targetPeriod = 0;

	return targetPeriod;
}

u16 APUSweep::calculateTargetPeriod2(u16 timerPeriod)
{
	u16 targetPeriod = timerPeriod;
	
	// Shift right
	u16 shiftedPeriod = timerPeriod >> mShiftCount;

	// Add/substract to current period 
	targetPeriod += !mIsNegating ? shiftedPeriod : -shiftedPeriod;

	// Clamp to 0 if negative
	if ((targetPeriod & 0b1000'0000'0000'0000) != 0)
		targetPeriod = 0;

	return targetPeriod;
}
