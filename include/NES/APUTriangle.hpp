#pragma once

#include <array>

#include "NES/APULengthCounter.hpp"
#include "NES/APUDivider.hpp"
#include "NES/APUFrameCounter.hpp"

class APUTriangle
{
public:
	void reset();

	void update(APUFrameCounterState fcState);
	void setReg0(u8 value);
	void setReg1(u8 value);
	void setReg2(u8 value);
	inline void enable() { mLengthCounter.enable(); }
	inline void disable() { mLengthCounter.disable(); }

	inline u8 getOutput() const { return mOutput; }
	inline bool getStatus() const { return mLengthCounter.getCounter() > 0; }

private:
	inline void incrementSequenceIndex()
	{
		mSequenceIndex++;
		if (mSequenceIndex >= 32)
			mSequenceIndex = 0;
	}

	static constexpr std::array<u8, 32> SEQUENCER_LUT = 
	{{
		15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0,
        0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
	}};
	APUDivider mTimer;
	APULengthCounter mLengthCounter;
	bool mIsControlFlagSet;
	bool mIsCounterReloadFlagSet;
	u8 mLinCounterReloadValue;
	u8 mLinearCounterValue;
	u8 mOutput;

	u8 mSequenceIndex;
};
