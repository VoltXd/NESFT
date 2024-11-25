#pragma once

#include "NES/CPU.hpp"
#include "NES/APU.hpp"
#include "NES/PPU.hpp"
#include "NES/Memory.hpp"
#include "NES/Controller.hpp"
#include "IO/SoundManager.hpp"

#include <string>

constexpr double FRAME_PERIOD_NTSC = 1.0 / 60.0988;

class NES
{
public:
    NES(Controller& controller, const std::string& romFilename);

    void reset();
    void runOneCpuInstruction();

	void prepareDrawnSoundBuffers();

	inline bool isImageReady() const { return mPpu.isImageReady(); }
	inline void clearIsImageReady() { mPpu.clearIsImageReady(); }
	inline const picture_t& getPicture() { return mPpu.getPicture(); }

	inline bool isSoundBufferReady() const { return mIsSoundBufferReady; }
	inline void clearIsSoundBufferReady() { mIsSoundBufferReady = false; }
	inline const soundBuffer_t* getSoundBufferPtr() const { return mSoundBufferToSubmit; }
	inline const soundBufferF32_t* getBufferToDrawPtr() const { return &mBufferToDraw; }
	inline const soundBufferF32_t* getP1BufferToDrawPtr() const { return &mP1BufferToDraw; }
	inline const soundBufferF32_t* getP2BufferToDrawPtr() const { return &mP2BufferToDraw; }
	inline const soundBufferF32_t* getTriangleBufferToDrawPtr() const { return &mTriangleBufferToDraw; }
	inline const soundBufferF32_t* getNoiseBufferToDrawPtr() const { return &mNoiseBufferToDraw; }
	inline const soundBufferF32_t* getDmcBufferToDrawPtr() const { return &mDmcBufferToDraw; }
	

private:
	void fillDrawnSoundBuffer(const soundBuffer_t& soundBuffer0, const soundBuffer_t& soundBuffer1, soundBufferF32_t& drawnSoundBuffer);
	
    CPU mCpu;
	APU mApu;
	PPU mPpu;
	Memory mMemory;
	
	bool mIsDmaGetCycle;
    
    // Sound
	static constexpr float TIME_PER_CYCLE = 1.0f / 1'789'773;
	bool mIsUsingSoundBuffer0;
	bool mIsSoundBufferReady;
	soundBuffer_t* mSoundBufferToSubmit;
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
};