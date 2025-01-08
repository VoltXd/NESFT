#include "NES/NES.hpp"

#include "NES/Toolbox.hpp"

NES::NES(Controller& controller1, Controller& controller2, const std::string &romFilename)
    : mMemory(romFilename, mApu, mPpu, controller1, controller2)
{
    // Power up == Reset
    reset();

	mSoundFIFO    = soundFIFO_t(BUFFER_SIZE);
	mP1FIFO       = soundFIFO_t(BUFFER_SIZE);
	mP2FIFO       = soundFIFO_t(BUFFER_SIZE);
	mTriangleFIFO = soundFIFO_t(BUFFER_SIZE);
	mNoiseFIFO    = soundFIFO_t(BUFFER_SIZE);
	mDmcFIFO      = soundFIFO_t(BUFFER_SIZE);

	mMasterVolume = 1.0f;
}

void NES::reset()
{   
	if (!mMemory.isRomPlayable())
		return;
		
	mMemory.reset();
	mApu.reset();
	mPpu.reset();
	mCpuCyclesElapsed = mCpu.reset(mMemory);
	mCpuCyclesPredicted = mCpuCyclesElapsed;


	mDmcDmaExtraCycles = 0;
	mIsDmaGetCycle = false;
	mApuTimestamp = 0.0f;
	mSoundSamplesCount = 0;
	mIsUsingSoundBuffer0 = true;
    mIsSoundBufferReady = false;

	mIsIrqSet = false;
	mIsNmiSet = false;

	// Run APU & PPU to keep up with CPU
	runApu();
	runPpu();
}

void NES::runOneCpuInstruction()
{
	// Run APU and PPU if they have to keep up
	if (mCpuCyclesElapsed > mCpuCyclesPredicted)
	{
		mCpuCyclesPredicted = mCpuCyclesElapsed - mCpuCyclesPredicted;
		runApu();
		runPpu();
	}

	// Poll NMI/IRQ
	pollIrqAndNmi();

	// Predict CPU cycles to go 
	mCpuCyclesPredicted = getCpuCyclesPrediction();

	// Run APU & PPU
	runApu();
	runPpu();

	// Run CPU
	runCpu();
}

s32 NES::getCpuCyclesPrediction()
{
	return mCpu.predictCyclesToRun(mMemory, mMemory.isOamDmaStarted(), mIsIrqSet, mIsNmiSet);
}

void NES::pollIrqAndNmi()
{
	mIsNmiSet = mPpu.getVBlankNMISignal();
	mIsIrqSet = mMemory.getCartridgeIrq()       ||
	            mApu.getFrameCounterIRQSignal() ||
				mApu.getDMCIRQSignal();
}

void NES::runApu()
{
	mDmcDmaExtraCycles = 0;
	for (int i = 0; i < mCpuCyclesPredicted + mDmcDmaExtraCycles; i++)
	{
		// Execute APU + get extra cycles due to DMC DMA
		mIsDmaGetCycle = !mIsDmaGetCycle;
		mDmcDmaExtraCycles += mApu.executeOneCpuCycle(mMemory, mIsDmaGetCycle);

		mApuTimestamp += TIME_PER_CYCLE;
		if (mApuTimestamp > BUFFER_SAMPLE_PERIOD)
		{
			// Get sample & substract sample period to time stamp
			soundBufferF32_t* soundBuffer = mIsUsingSoundBuffer0 ? &mSoundBuffer0 : &mSoundBuffer1;
			
			float apuOutput = limitToInterval(mMasterVolume * mApu.getOutput(), 0.0f, 1.0f);
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
	else if(mIsNmiSet)
	{
		mCpuCyclesElapsed = mCpu.nmi(mMemory);
		mPpu.clearNMISignal();	
	}
	else if (mIsIrqSet)
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
	for (int i = 0; i < 3 * (mCpuCyclesPredicted + mDmcDmaExtraCycles); i++)
		mPpu.executeOneCycle(mMemory);
}
