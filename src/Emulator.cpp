#include "Emulator.hpp"

#include <cstdlib>
#include <iostream>
#include <chrono>
#include "NES/Cartridge.hpp"
#include "NES/Toolbox.hpp"
#include "IO/GlfwApp.hpp"

#include "IO/SoundManager.hpp"
#include <cmath>

Emulator::Emulator(const std::string &romFilename)
	: mMemory(romFilename, mApu, mPpu, mController)
{
	// Reset
	mMemory.reset();
	mCpu.reset(mMemory);
	mApu.reset();
	mPpu.reset();

	mIsDmaGetCycle = false;
	mApuTimestamp = 0.0f;
	mSoundSamplesCount = 0;
	mIsUsingSoundBuffer0 = true;

	testAndExitWithMessage(mSoundManager.initialise() == EXIT_FAILURE, "Cannot initialise sound manager...");
}

int Emulator::run()
{
	// *************** NES Emulation *************** //
	GlfwApp appWindow(mController);

	std::chrono::steady_clock::time_point time0 = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point time1;
	double elapsedTime, elapsedTimeOffset = 0;

	// Infinite loop
	while (!appWindow.shouldWindowClose())
	{
		// Update
		runOneInstruction();
		if (mPpu.isImageReady())
		{
			mPpu.clearIsImageReady();

			// Wait before rendering -> 60 FPS
			do
			{
				time1 = std::chrono::steady_clock::now();
				elapsedTime = std::chrono::duration<double>(time1 - time0).count();

			} while (elapsedTime + elapsedTimeOffset < FRAME_PERIOD_NTSC);
			time0 = time1;
			elapsedTimeOffset = elapsedTime + elapsedTimeOffset - FRAME_PERIOD_NTSC;
			
			// Render
			appWindow.draw(mPpu.getPicture());
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
	else if (mApu.getFrameCounterIRQSignal())
	{
		elapsedCycles = mCpu.irq(mMemory);
		mApu.clearIRQSignal();
	}
	else
		elapsedCycles = mCpu.execute(1, mMemory);

	// Update get/put cycle flag (DMA)
	mIsDmaGetCycle = (bool)(((mIsDmaGetCycle ? 1 : 0) + elapsedCycles) % 2);
	
	// APU
	for (int i = 0; i < elapsedCycles; i++)
	{
		mApu.executeOneCpuCycle();
		mApuTimestamp += TIME_PER_CYCLE;
		if (mApuTimestamp > BUFFER_SAMPLE_PERIOD)
		{
			// Get sample & substract sample period to time stamp
			soundBuffer_t* soundBuffer = mIsUsingSoundBuffer0 ? &mSoundBuffer0 : &mSoundBuffer1;
			(*soundBuffer)[mSoundSamplesCount++] = (u8)(256 * mApu.getOutput());

			mApuTimestamp -= BUFFER_SAMPLE_PERIOD;

			// Push buffer to audio output when full
			if (mSoundSamplesCount >= BUFFER_SIZE)
			{
				mSoundSamplesCount = 0;
				mIsUsingSoundBuffer0 = !mIsUsingSoundBuffer0;
				while (mSoundManager.streamSound(*soundBuffer) == StreamStatus::NOT_QUEUED);
			}
		}
	}

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
