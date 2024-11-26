#include "NES/NES.hpp"

#include "NES/Toolbox.hpp"

NES::NES(Controller& controller, const std::string &romFilename)
    : mMemory(romFilename, mApu, mPpu, controller)
{
    // Power up == Reset
    reset();
	mSoundFIFO    = soundFIFO_t(BUFFER_SIZE);
	mP1FIFO       = soundFIFO_t(BUFFER_SIZE);
	mP2FIFO       = soundFIFO_t(BUFFER_SIZE);
	mTriangleFIFO = soundFIFO_t(BUFFER_SIZE);
	mNoiseFIFO    = soundFIFO_t(BUFFER_SIZE);
	mDmcFIFO      = soundFIFO_t(BUFFER_SIZE);
}

void NES::reset()
{   
	mMemory.reset();
	mCpu.reset(mMemory);
	mApu.reset();
	mPpu.reset();

	mIsDmaGetCycle = false;
	mApuTimestamp = 0.0f;
	mSoundSamplesCount = 0;
	mIsUsingSoundBuffer0 = true;
    mIsSoundBufferReady = false;
}

void NES::runOneCpuInstruction()
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
			soundBufferF32_t* soundBuffer = mIsUsingSoundBuffer0 ? &mSoundBuffer0 : &mSoundBuffer1;
			float apuOutput = mApu.getOutput();
			(*soundBuffer)[mSoundSamplesCount] = apuOutput;

			// Get sample per channel (TODO: fix caches misses ?)
			popAndPush(mSoundFIFO, apuOutput);
			popAndPush(mP1FIFO, mApu.getPulse1Output());
			popAndPush(mP2FIFO, mApu.getPulse2Output());
			popAndPush(mTriangleFIFO, mApu.getTriangleOutput());
			popAndPush(mNoiseFIFO, mApu.getNoiseOutput());
			popAndPush(mDmcFIFO, mApu.getDMCOutput());

			mSoundSamplesCount++;
			mApuTimestamp -= BUFFER_SAMPLE_PERIOD;

			// Prepare buffer to be sent to the sound manager
			if (mSoundSamplesCount >= BUFFER_SIZE)
			{
				mSoundSamplesCount = 0;
				mIsUsingSoundBuffer0 = !mIsUsingSoundBuffer0;
				mSoundBufferToSubmit = soundBuffer;
                mIsSoundBufferReady = true;
			}
		}
	}

	// PPU
	for (int i = 0; i < 3 * (elapsedCycles + extraCycles); i++)
		mPpu.executeOneCycle(mMemory);
}
