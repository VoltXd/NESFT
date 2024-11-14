#pragma once

#include <string>

#include "NES/CPU.hpp"
#include "NES/APU.hpp"
#include "NES/PPU.hpp"
#include "NES/Memory.hpp"
#include "IO/SoundManager.hpp"

class Emulator
{
public:
	Emulator(const std::string& romFilename);
	
	int run();

private:
	void runOneInstruction();
	void printCpuInfo(u16 pc, u8 instruction, s32 elapsedCycles);

	// NES Components
	static constexpr double FRAME_PERIOD_NTSC = 1.0 / 60.0988;
	CPU mCpu;
	APU mApu;
	PPU mPpu;
	Memory mMemory;
	Controller mController;

	bool mIsDmaGetCycle;

	// Sound 
	static constexpr float TIME_PER_CYCLE = 1.0f / 1'789'773;
	soundBuffer_t mSoundBuffer0;
	soundBuffer_t mSoundBuffer1;
	bool mIsUsingSoundBuffer0;
	float mApuTimestamp;
	u16 mSoundSamplesCount;

	SoundManager mSoundManager;
};
