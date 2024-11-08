#include "Emulator.hpp"

#include <cstdlib>
#include <iostream>
#include "NES/Cartridge.hpp"
#include "IO/GlfwRenderer.hpp"

Emulator::Emulator(const std::string &romFilename)
	: mMemory(romFilename, mPpu)
{
}

int Emulator::run()
{
	// *************** NES Emulation *************** //
	// Reset
	mMemory.reset();
	mCpu.reset(mMemory);
	mPpu.reset();
	mIsDmaGetCycle = false;

	GlfwRenderer renderer;
		
	// Infinite loop
	while (true)
	{
		runOneInstruction();
		if (mPpu.isImageReady())
		{
			mPpu.clearIsImageReady();
			renderer.draw(mPpu.getPicture());
		}
	}

	return EXIT_SUCCESS;
}

void Emulator::runOneInstruction()
{
	// u16 pc = mCpu.getPc();
	// u8 instruction = mMemory.cpuRead(pc);

	// CPU
	s32 elapsedCycles;
	if (mMemory.isOamDmaStarted())
		elapsedCycles = mMemory.executeOamDma(mIsDmaGetCycle);
	else if(mPpu.getVBlankNMISignal())
	{
		elapsedCycles = mCpu.nmi(mMemory);
		mPpu.clearNMISignal();	
	}
	else
		elapsedCycles = mCpu.execute(1, mMemory);

	// Update get/put cycle flag (DMA)
	mIsDmaGetCycle = (bool)(((mIsDmaGetCycle ? 1 : 0) + elapsedCycles) % 2);
	
	// PPU
	for (int i = 0; i < 3 * elapsedCycles; i++)
		mPpu.executeOneCycle(mMemory);

	// printCpuInfo(pc, instruction, elapsedCycles);
}

void Emulator::printCpuInfo(u16 pc, u8 instruction, s32 elapsedCycles)
{
	std::cout << std::uppercase << std::hex 
	          << "PC: 0x" << pc
	          << ", INST: 0x" << +instruction
			  << std::dec << ", cycles: " << +elapsedCycles 
			  << std::endl;
}
