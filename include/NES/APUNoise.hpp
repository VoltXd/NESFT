#pragma once

#include <array>

#include "NES/APUEnvelopeGenerator.hpp"
#include "NES/APULengthCounter.hpp"
#include "NES/Divider.hpp"
#include "NES/APUFrameCounter.hpp"

class APUNoise
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
	static constexpr std::array<u16, 0x10> TIMER_NTSC_PERIOD_LUT = 
	{{
		2, 4, 8, 16, 32, 48, 64, 80, 101, 127, 190, 254, 381, 508, 1017, 2034
	}};

	APUEnvelopeGenerator mEnvelope;
	Divider mTimer;
	APULengthCounter mLengthCounter;
	u16 mShiftRegister;
	u8 mOutput;

	bool mIsModeFlagSet;
};
