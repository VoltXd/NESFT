#include "NES/APUDivider.hpp"

void APUDivider::reset()
{
	mCounter = 0;
	mPeriod = 0;
}

bool APUDivider::countDown()
{
	bool hasFinishedCycle = false;
	
	if (mCounter == 0)
	{
		reloadCounter();
		hasFinishedCycle = true;
	}
	else
		mCounter--;
	
	return hasFinishedCycle;
}