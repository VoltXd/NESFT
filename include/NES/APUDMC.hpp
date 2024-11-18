#pragma once

#include <array>

#include "NES/Config.hpp"
#include "NES/APUDivider.hpp"
#include "NES/Memory.hpp"

class APUDMC
{
public:
	void reset();

	s32 update(Memory& memory, bool isGetCycle);
	void setReg0(u8 value);
	void setReg1(u8 value);
	void setReg2(u8 value);
	void setReg3(u8 value);
	void enable();
	void disable();

	inline bool getStatus() const { return mMemReaderCount > 0; }
	inline u8 getOutput() const { return mOutput; }
	inline bool getIRQSignal() const { return mIsIRQSignalSet; }

	inline void clearIRQSignal() { mIsIRQSignalSet = false; }

private:
	u8 dmaRead(Memory& memory, bool isGetCycle, s32& extraCycles);

	std::array<u16, 16> RATE_LUT_NTSC = 
	{{
		428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54
	}};

	// Timer & registers
	APUDivider mTimer;
	bool mIsIRQSet;
	bool mIsIRQSignalSet;
	bool mIsLooping;
	u8 mRateIndex;

	u16 mSampleAddress;
	u16 mSampleLength;

	// Memory reader
	u16 mMemReaderAddress;
	u16 mMemReaderCount;

	// Sample buffer
	u8 mSampleBuffer;
	bool mIsBufferFull;

	// Output unit
	u8 mShiftRegister;
	u8 mShifterBitsRemaining;
	u8 mOutput;
	bool mIsSilenced;
};