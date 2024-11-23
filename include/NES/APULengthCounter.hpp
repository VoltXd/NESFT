#pragma once

#include <array>

#include "NES/Config.hpp"

class APULengthCounter
{
public:
	void reset();	
	
	void enable();
	void disable();
	void update();

	inline void setCounter(u8 index) 
	{ 
		if (mIsEnabled) 
			mCounter = LENGTH_LUT[index]; 
	}
	inline void setHaltFlag(bool flag) { mIsHaltSet = flag; }

	inline u8 getCounter() const { return mCounter; }

private:
	static constexpr std::array<u8, 0x20> LENGTH_LUT = 
	{{
		10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
		12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
	}};

	u8 mCounter;
	bool mIsHaltSet;
	bool mIsEnabled;
};
