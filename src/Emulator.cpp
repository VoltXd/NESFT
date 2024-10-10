#include "Emulator.hpp"

#include <cstdlib>
#include <iostream>
#include "Cartridge.hpp"

Emulator::Emulator(const std::string &romFilename)
	: mMemory(romFilename)
{
}

int Emulator::run()
{
	// Instantiations
	
	// *************** CPU Emulation *************** //
	// Reset
	mMemory.reset();
	mCpu.reset(mMemory);

	// Infinite loop
	while (true)
	{
		runOneCycleAndPrintCPUInfo();
	}

	return EXIT_SUCCESS;
}

void Emulator::runOneCycleAndPrintCPUInfo()
{
	word pc = mCpu.getPc();
	byte instruction = mMemory.cpuRead(pc);

	sdword elapsedCycles = mCpu.execute(1, mMemory);

	std::cout << std::uppercase << std::hex 
	          << "PC: 0x" << pc
	          << ", INST: 0x" << +instruction
			  << std::dec << ", cycles: " << +elapsedCycles 
			  << std::endl;
}
