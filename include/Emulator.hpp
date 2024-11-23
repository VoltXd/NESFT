#pragma once

#include <string>

#include "NES/CPU.hpp"
#include "NES/APU.hpp"
#include "NES/PPU.hpp"
#include "NES/Memory.hpp"
#include "IO/GlfwApp.hpp"
#include "IO/SoundManager.hpp"

class Emulator
{
public:
	Emulator(const std::string& romFilename);
	
	int run();

private:
	void runOneInstruction();
	void fillBuffersAndSendToWindow(GlfwApp& window);
	void fillDrawnSoundBuffer(const soundBuffer_t& soundBuffer0, const soundBuffer_t& soundBuffer1, soundBufferF32_t& soundDrawnBuffer);

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
	bool mIsUsingSoundBuffer0;
	soundBuffer_t mSoundBuffer0;
	soundBuffer_t mSoundBuffer1;
	soundBuffer_t mP1Buffer0;
	soundBuffer_t mP1Buffer1;
	soundBuffer_t mP2Buffer0;
	soundBuffer_t mP2Buffer1;
	soundBuffer_t mTriangleBuffer0;
	soundBuffer_t mTriangleBuffer1;
	soundBuffer_t mNoiseBuffer0;
	soundBuffer_t mNoiseBuffer1;
	soundBuffer_t mDmcBuffer0;
	soundBuffer_t mDmcBuffer1;
	soundBufferF32_t mBufferToDraw;
	soundBufferF32_t mP1BufferToDraw;
	soundBufferF32_t mP2BufferToDraw;
	soundBufferF32_t mTriangleBufferToDraw;
	soundBufferF32_t mNoiseBufferToDraw;
	soundBufferF32_t mDmcBufferToDraw;
	float mApuTimestamp;
	u16 mSoundSamplesCount;

	SoundManager mSoundManager;
};
