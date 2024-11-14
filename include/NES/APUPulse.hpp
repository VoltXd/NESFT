#pragma once

#include <array>

#include "NES/APUEnvelopeGenerator.hpp"
#include "NES/APULengthCounter.hpp"
#include "NES/APUDivider.hpp"
#include "NES/APUFrameCounter.hpp"
#include "NES/APUSweep.hpp"

class APUPulse
{
public:
	APUPulse(bool isPulse1);
	void reset();

	void update(APUFrameCounterState fcState);
	void setReg0(u8 value);
	void setReg1(u8 value);
	void setReg2(u8 value);
	void setReg3(u8 value);
	inline void enable() { mLengthCounter.enable(); }
	inline void disable() { mLengthCounter.disable(); }

	inline u8 getOutput() const { return mOutput; }


private:
	inline void incrementSequenceIndex()
	{
		mSequenceIndex++;
		if (mSequenceIndex >= 8)
			mSequenceIndex = 0;
	}

	static constexpr std::array<std::array<u8, 8>, 4> SEQUENCER_LUT = 
	{{
		{ 0, 0, 0, 0, 0, 0, 0, 1, },
		{ 0, 0, 0, 0, 0, 0, 1, 1, },
		{ 0, 0, 0, 0, 1, 1, 1, 1, },
		{ 1, 1, 1, 1, 1, 1, 0, 0, }
	}};

	APUEnvelopeGenerator mEnvelope;
	APUDivider mTimer;
	APULengthCounter mLengthCounter;
	APUSweep mSweep;
	u8 mOutput;

	bool mIsPulse1;
	u8 mDutyCycle;
	u8 mSequenceIndex;
};