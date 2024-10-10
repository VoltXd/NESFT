#pragma once

#include <string>

#include "CPU.hpp"
#include "Memory.hpp"

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
