#pragma once

#include <string>

#include "NES/CPU.hpp"
#include "NES/APU.hpp"
#include "NES/PPU.hpp"
#include "NES/Memory.hpp"

class Emulator
{
public:
	Emulator(const std::string& romFilename);
	
	int run();

private:
	void runOneInstruction();
	void printCpuInfo(u16 pc, u8 instruction, s32 elapsedCycles);

	CPU mCpu;
	APU mApu;
	PPU mPpu;
	Memory mMemory;
	Controller mController;

	bool mIsDmaGetCycle;
};
