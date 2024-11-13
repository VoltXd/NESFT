#include "NES/APUEnvelopeGenerator.hpp"

void APUEnvelopeGenerator::reset()
{
	// Set up the decay counter
	mDecayCounter = DECAY_COUNTER_PERIOD;
}

void APUEnvelopeGenerator::onClock()
{
	bool isDividerClockedWhile0 = false;

	// Update divider
	if (!mIsStartFlagClear)
	{
		mIsStartFlagClear = true;
		mDecayCounter = DECAY_COUNTER_PERIOD;
		mDivider.reloadCounter();
	}	
	else
		// Clock the divider
		isDividerClockedWhile0 = mDivider.countDown();
	
	if (isDividerClockedWhile0)
	{
		// Update decay counter
		if (mDecayCounter == 0)
		{
			if (mIsLoopFlagSet)
				mDecayCounter = DECAY_COUNTER_PERIOD;	
		}
		else
			mDecayCounter--;
	}	
}

void APUEnvelopeGenerator::setVolume(u8 volume)
{
	// Store the volume (constant volume)
	mVolume = volume;

	// And set divider period (decay)
	mDivider.loadPeriod(mVolume);
}