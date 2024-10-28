#include "Emulator.hpp"

#include <cstdlib>
#include <iostream>
#include "NES/Cartridge.hpp"
#include "IO/GlfwRenderer.hpp"

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

	GlfwRenderer renderer;
	picture_t redScreen = { 0 }; 
	for (int i = 0; i < PPU_OUTPUT_HEIGHT; i++)
		for (int j = 0; j < PPU_OUTPUT_WIDTH; j++)
		{
			redScreen[i][j][0] = (byte)i;
			redScreen[i][j][1] = (byte)j;
			redScreen[i][j][2] = (byte)sqrt(i * j);
		}
	
		
	// Infinite loop
	while (true)
	{
		runOneCycleAndPrintCPUInfo();
		renderer.draw(redScreen);
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
