#pragma once

#include <string>

#include "NES/CPU.hpp"
#include "NES/PPU.hpp"
#include "NES/Memory.hpp"

class Emulator
{
public:
	Emulator(const std::string& romFilename);
	
	int run();

private:
	void runOneCycleAndPrintCPUInfo();

	CPU mCpu;
	Memory mMemory;
};
