#pragma once 

#include "NES/Config.hpp"
#include "NES/APUDivider.hpp"

enum class APUFrameCounterState
{
	IDLE,
	QUARTER,
	HALF
};

class APUFrameCounter
{
public:
	void reset();

	APUFrameCounterState executeOneCpuCycle();
	void writeRegister(u8 reg);

	inline bool isEvenCycle() const { return mIsEvenCycle; }
	inline bool getIRQSignal() const { return mIsIRQSignalSet; }

	inline void clearIRQSignal() { mIsIRQSignalSet = false; }

private:
	// NTSC cycles for sequence steps
	static constexpr u16 FC_STEP1_CYCLE_COUNT = 3728; 
	static constexpr u16 FC_STEP2_CYCLE_COUNT = 7456; 
	static constexpr u16 FC_STEP3_CYCLE_COUNT = 11185; 
	static constexpr u16 FC_STEP4_CYCLE_COUNT = 14914; 
	static constexpr u16 FC_STEP5_CYCLE_COUNT = 18640; 

	APUDivider mApuClockDivider;

	s16 mCycleCount;
	bool mIs5StepsMode;
	bool mIsInterruptInhibited;
	bool mIsIRQSignalSet;
	bool mIsTimerReset;
	bool mIsEvenCycle;
	bool mIsEndReached;
};
