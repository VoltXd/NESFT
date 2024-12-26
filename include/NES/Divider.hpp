#pragma once

#include "NES/Config.hpp"

class Divider
{
public:
	void reset();
	
	inline void reloadCounter() { mCounter = mPeriod; }
	inline void loadPeriod(u16 period) { mPeriod = period; } 

	inline u16 getCounter() { return mCounter; }
	inline u16 getPeriod() { return mPeriod; }

	bool countDown();
	bool registerShift();

private:
	u16 mPeriod;
	u16 mCounter;
};