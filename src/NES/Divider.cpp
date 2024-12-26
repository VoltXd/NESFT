#include "NES/Divider.hpp"

void Divider::reset()
{
	mCounter = 0;
	mPeriod = 0;
}

bool Divider::countDown()
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