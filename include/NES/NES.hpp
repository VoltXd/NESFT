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
	~NES() { reset(); }

    void reset();
    void runOneCpuInstruction();

	inline bool isImageReady() const { return mPpu.isImageReady(); }
	inline void clearIsImageReady() { mPpu.clearIsImageReady(); }
	inline const picture_t& getPicture() { return mPpu.getPicture(); }

	inline bool isSoundBufferReady() const { return mIsSoundBufferReady; }
	inline void clearIsSoundBufferReady() { mIsSoundBufferReady = false; }
	inline const soundBufferF32_t* getSoundBufferPtr() const { return mSoundBufferToSubmit; }
	inline const soundFIFO_t* getSoundFIFOPtr() const { return &mSoundFIFO; }
	inline const soundFIFO_t* getP1FIFOPtr() const { return &mP1FIFO; }
	inline const soundFIFO_t* getP2FIFOPtr() const { return &mP2FIFO; }
	inline const soundFIFO_t* getTriangleFIFOPtr() const { return &mTriangleFIFO; }
	inline const soundFIFO_t* getNoiseFIFOPtr() const { return &mNoiseFIFO; }
	inline const soundFIFO_t* getDmcFIFOPtr() const { return &mDmcFIFO; }
	
	inline bool isRomPlayable() const { return mMemory.isRomPlayable(); }
	inline const std::string& getErrorMessage() const { return mMemory.getErrorMessage(); }
	inline const std::string& getHeaderInfo() const { return mMemory.getHeaderInfo(); }

private:
	s32 getCpuCyclesPrediction();
	void pollIrqAndNmi();
	void runApu();
	void runCpu();
	void runPpu();

    CPU mCpu;
	APU mApu;
	PPU mPpu;
	Memory mMemory;

	s32 mCpuCyclesPredicted;
	s32 mCpuCyclesElapsed;
	s32 mDmcDmaExtraCycles;
	
	bool mIsDmaGetCycle;

	bool mIsNmiSet;
	bool mIsIrqSet;
    
    // Sound
	static constexpr float TIME_PER_CYCLE = 1.0f / 1'789'773;
	bool mIsUsingSoundBuffer0;
	bool mIsSoundBufferReady;
	soundBufferF32_t* mSoundBufferToSubmit;
	soundBufferF32_t mSoundBuffer0;
	soundBufferF32_t mSoundBuffer1;
	soundFIFO_t mSoundFIFO;
	soundFIFO_t mP1FIFO;
	soundFIFO_t mP2FIFO;
	soundFIFO_t mTriangleFIFO;
	soundFIFO_t mNoiseFIFO;
	soundFIFO_t mDmcFIFO;
	float mApuTimestamp;
	u16 mSoundSamplesCount;
};