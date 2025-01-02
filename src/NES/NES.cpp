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
	mApu.reset();
	mPpu.reset();
	mCpuCyclesElapsed = mCpu.reset(mMemory);


	mDmcDmaExtraCycles = 0;
	mIsDmaGetCycle = false;
	mApuTimestamp = 0.0f;
	mSoundSamplesCount = 0;
	mIsUsingSoundBuffer0 = true;
    mIsSoundBufferReady = false;

	// Run APU & PPU to keep up with CPU
	runApu();
	runPpu();
}

void NES::runOneCpuInstruction()
{
	// TODO: Predict CPU cycles to go

	// Run APU & PPU
	runApu();
	runPpu();

	// Run CPU
	runCpu();
}

void NES::runApu()
{
	mDmcDmaExtraCycles = 0;
	for (int i = 0; i < mCpuCyclesElapsed + mDmcDmaExtraCycles; i++)
	{
		// Execute APU + get extra cycles due to DMC DMA
		mIsDmaGetCycle = !mIsDmaGetCycle;
		mDmcDmaExtraCycles += mApu.executeOneCpuCycle(mMemory, mIsDmaGetCycle);
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
}

void NES::runCpu()
{
	mCpuCyclesElapsed = 0;
	if (mMemory.isOamDmaStarted())
	{
		mCpuCyclesElapsed = mMemory.executeOamDma(mIsDmaGetCycle);
	}
	else if(mPpu.getVBlankNMISignal())
	{
		mCpuCyclesElapsed = mCpu.nmi(mMemory);
		mPpu.clearNMISignal();	
	}
	else if (mMemory.getCartridgeIrq()) 
	{
		mCpuCyclesElapsed = mCpu.irq(mMemory);
	}
	else if (mApu.getFrameCounterIRQSignal() || 
	         mApu.getDMCIRQSignal())
	{
		mCpuCyclesElapsed = mCpu.irq(mMemory);
	}
	
	if (mCpuCyclesElapsed == 0)
	{
		// No IRQ, NMI, or OAM DMA has been executed
		// -> execute one instruction
		mCpuCyclesElapsed = mCpu.execute(1, mMemory);
	}
}

void NES::runPpu()
{
	// PPU
	for (int i = 0; i < 3 * (mCpuCyclesElapsed + mDmcDmaExtraCycles); i++)
		mPpu.executeOneCycle(mMemory);
}
