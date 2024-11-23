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
		if (appWindow.isPaused())
		{
			appWindow.draw(mPpu.getPicture());
			time0 = std::chrono::steady_clock::now();
			continue;
		}

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

			if (appWindow.isSoundChannelsWindowOpen())
				fillBuffersAndSendToWindow(appWindow);
						
			// Render
			appWindow.draw(mPpu.getPicture());
		}
	}

	return EXIT_SUCCESS;
}

void Emulator::runOneInstruction()
{
	// CPU
	s32 elapsedCycles;
	if (mMemory.isOamDmaStarted())
		elapsedCycles = mMemory.executeOamDma(mIsDmaGetCycle);
	else if(mPpu.getVBlankNMISignal())
	{
		elapsedCycles = mCpu.nmi(mMemory);
		mPpu.clearNMISignal();	
	}
	else if (mApu.getFrameCounterIRQSignal() || mApu.getDMCIRQSignal())
	{
		elapsedCycles = mCpu.irq(mMemory);
		mApu.clearIRQSignal();
	}
	else
		elapsedCycles = mCpu.execute(1, mMemory);

	// Update get/put cycle flag (DMA)
	mIsDmaGetCycle = (bool)(((mIsDmaGetCycle ? 1 : 0) + elapsedCycles) % 2);
	
	// APU
	s32 extraCycles = 0;
	for (int i = 0; i < (elapsedCycles + extraCycles); i++)
	{
		// Execute APU + get extra cycles due to DMC DMA
		extraCycles = mApu.executeOneCpuCycle(mMemory, mIsDmaGetCycle);
		mApuTimestamp += TIME_PER_CYCLE;
		if (mApuTimestamp > BUFFER_SAMPLE_PERIOD)
		{
			// Get sample & substract sample period to time stamp
			soundBuffer_t* soundBuffer = mIsUsingSoundBuffer0 ? &mSoundBuffer0 : &mSoundBuffer1;
			(*soundBuffer)[mSoundSamplesCount] = (u8)(0.5 * 255 * (mApu.getOutput() + 1));

			// Get sample per channel (TODO: fix caches misses ?)
			soundBuffer_t* p1Buffer = mIsUsingSoundBuffer0 ? &mP1Buffer0 : &mP1Buffer1;
			(*p1Buffer)[mSoundSamplesCount] = (u8)(0.5 * 255 * (mApu.getPulse1Output() + 1));

			soundBuffer_t* p2Buffer = mIsUsingSoundBuffer0 ? &mP2Buffer0 : &mP2Buffer1;
			(*p2Buffer)[mSoundSamplesCount] = (u8)(0.5 * 255 * (mApu.getPulse2Output() + 1));
			
			soundBuffer_t* triangleBuffer = mIsUsingSoundBuffer0 ? &mTriangleBuffer0 : &mTriangleBuffer1;
			(*triangleBuffer)[mSoundSamplesCount] = (u8)(0.5 * 255 * (mApu.getTriangleOutput() + 1));

			soundBuffer_t* noiseBuffer = mIsUsingSoundBuffer0 ? &mNoiseBuffer0 : &mNoiseBuffer1;
			(*noiseBuffer)[mSoundSamplesCount] = (u8)(0.5 * 255 * (mApu.getNoiseOutput() + 1));
			
			soundBuffer_t* dmcBuffer = mIsUsingSoundBuffer0 ? &mDmcBuffer0 : &mDmcBuffer1;
			(*dmcBuffer)[mSoundSamplesCount] = (u8)(0.5 * 255 * (mApu.getDMCOutput() + 1));

			mSoundSamplesCount++;
			mApuTimestamp -= BUFFER_SAMPLE_PERIOD;

			// Push buffer to audio output when full
			if (mSoundSamplesCount >= BUFFER_SIZE)
			{
				mSoundSamplesCount = 0;
				mIsUsingSoundBuffer0 = !mIsUsingSoundBuffer0;
				// while (mSoundManager.streamSound(*soundBuffer) == StreamStatus::NOT_QUEUED);
				mSoundManager.streamSound(*soundBuffer);
			}
		}
	}

	// PPU
	for (int i = 0; i < 3 * (elapsedCycles + extraCycles); i++)
		mPpu.executeOneCycle(mMemory);
}

void Emulator::fillBuffersAndSendToWindow(GlfwApp& window)
{
	// Fill a sound buffer with latest data
	fillDrawnSoundBuffer(mSoundBuffer0, mSoundBuffer1, mBufferToDraw);
	fillDrawnSoundBuffer(mP1Buffer0, mP1Buffer1, mP1BufferToDraw);
	fillDrawnSoundBuffer(mP2Buffer0, mP2Buffer1, mP2BufferToDraw);
	fillDrawnSoundBuffer(mTriangleBuffer0, mTriangleBuffer1, mTriangleBufferToDraw);
	fillDrawnSoundBuffer(mNoiseBuffer0, mNoiseBuffer1, mNoiseBufferToDraw);
	fillDrawnSoundBuffer(mDmcBuffer0, mDmcBuffer1, mDmcBufferToDraw);

	window.setSoundBufferPtr(&mBufferToDraw);
	window.setP1BufferPtr(&mP1BufferToDraw);
	window.setP2BufferPtr(&mP2BufferToDraw);
	window.setTriangleBufferPtr(&mTriangleBufferToDraw);
	window.setNoiseBufferPtr(&mNoiseBufferToDraw);
	window.setDmcBufferPtr(&mDmcBufferToDraw);
}

void Emulator::fillDrawnSoundBuffer(const soundBuffer_t &soundBuffer0, const soundBuffer_t &soundBuffer1, soundBufferF32_t &soundDrawnBuffer)
{
	const soundBuffer_t* bufferPtr = mIsUsingSoundBuffer0 ? &soundBuffer1 : &soundBuffer0;
	for (u32 i = 0; i < (BUFFER_SIZE - mSoundSamplesCount); i++)
		soundDrawnBuffer[i] = (*bufferPtr)[i + mSoundSamplesCount];
	
	bufferPtr = mIsUsingSoundBuffer0 ? &soundBuffer0 : &soundBuffer1;
	for (u32 i = 0; i < mSoundSamplesCount; i++)
		soundDrawnBuffer[i + (BUFFER_SIZE - mSoundSamplesCount)] = (*bufferPtr)[i];
}
