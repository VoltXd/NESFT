#include "NES/APULengthCounter.hpp"

void APULengthCounter::reset()
{
	disable();
	mIsHaltSet = false;
}

void APULengthCounter::enable()
{
	mIsEnabled = true;
}

void APULengthCounter::disable()
{
	mIsEnabled = false;
	mCounter = 0;
}

void APULengthCounter::update()
{
	if ((!mIsHaltSet) && (mCounter != 0))
		mCounter--;
}