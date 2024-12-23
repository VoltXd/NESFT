#include "NES/APUDMC.hpp"

void APUDMC::reset()
{
	mTimer.reset();
	mIsIRQSet = false;
	mIsIRQSignalSet = false;
	mIsLooping = false;
	mRateIndex = 0;
	mSampleAddress = 0xC0'00;
	mSampleLength = 0x0'01;

	mMemReaderAddress = 0;
	mMemReaderCount = 0;

	mSampleBuffer = 0;
	mIsBufferFull = false;

	mShiftRegister = 0;
	mShifterBitsRemaining = 0;
	mOutput = 0;
}

s32 APUDMC::update(Memory& memory, bool isGetCycle)
{
	s32 extraCycles = 0;

	// Memory reader
	if (!mIsBufferFull && mMemReaderCount > 0)
	{
		// Buffer empty:
		// - Read DMA (1 byte) (TODO: load & reload DMA)
		// - Fill buffer
		// - memRead@++
		// - memByteCount--
		//   - if 0 && loop -> Restart sample
		//   - else if 0 -> IRQ
		mSampleBuffer = dmaRead(memory, isGetCycle, extraCycles);
		mIsBufferFull = true;
		incrementReaderAddress();
		mMemReaderCount--;

		if (mMemReaderCount == 0 && mIsLooping)
		{
			mMemReaderAddress = mSampleAddress;
			mMemReaderCount = mSampleLength;
		}
		else if (mMemReaderCount == 0 && mIsIRQSet)
		{
			mIsIRQSignalSet = true;
		}
	}
	
	// Timer
	bool isTimerClocking = mTimer.countDown();
	
	// Output unit
	if (isTimerClocking)
	{
		// In/Decrement output by 2
		// only when limits are not crossed [0, 127]
		if (!mIsSilenced)
		{
			s8 increment = ((mShiftRegister & 1) != 0) ? 2 : -2;
			s16 tempOutput = (s16)mOutput + increment;
			if (0 <= tempOutput  && tempOutput <= 127)
				mOutput = (u8)tempOutput;
		}

		// Shift right
		mShiftRegister >>= 1;

		// Decrement bit count
		mShifterBitsRemaining--;
		if (mShifterBitsRemaining == 0)
		{
			// Restart cycle
			mShifterBitsRemaining = 8;

			if (!mIsBufferFull)
			{
				mIsSilenced = true;
			}
			else
			{
				mIsSilenced = false;
				mShiftRegister = mSampleBuffer;
				mIsBufferFull = false;
			}
		}
	}

	return extraCycles;
}

void APUDMC::setReg0(u8 value)
{
	// reg0: IL-- RRRR
	mIsIRQSet = (value & 0b1000'0000) != 0;
	mIsLooping = (value & 0b0100'0000) != 0;

	if (!mIsIRQSet)
		clearIRQSignal();

	mRateIndex = value & 0b0000'1111;
	mTimer.loadPeriod(RATE_LUT_NTSC[mRateIndex]);
}

void APUDMC::setReg1(u8 value)
{
	// reg1: -DDD DDDD
	// There should be a hardware bug when this register
	// is written and when DMC timer clocks. In my emulator
	// it should not happen since there is no parallel 
	// computing.
	mOutput = value & 0b01111'1111;
}

void APUDMC::setReg2(u8 value)
{
	// reg2: AAAA AAAA
	mSampleAddress = 0xC000 | (value << 6);
}

void APUDMC::setReg3(u8 value)
{
	// reg3: LLLL LLLL
	mSampleLength = 1 | (value << 4);
}

void APUDMC::enable()
{
	clearIRQSignal();
	if (mMemReaderCount == 0)
	{
		mMemReaderAddress = mSampleAddress;
		mMemReaderCount = mSampleLength;
	}
}

void APUDMC::disable()
{
	clearIRQSignal();
	mMemReaderCount = 0;
}

u8 APUDMC::dmaRead(Memory &memory, bool isGetCycle, s32& extraCycles)
{
	// Get sample
	u8 sample = memory.cpuRead(mMemReaderAddress);

	// Calculate extra cycles
	// I will not take Load/Reload into account
	// -> Less precise but might not be 100 % relevent to 
	//    spend time on this
	extraCycles = isGetCycle ? 3 : 4;

	return sample;
}

inline void APUDMC::incrementReaderAddress()
{
	if (mMemReaderAddress != 0xFFFF)
		mMemReaderAddress++;
	else
		mMemReaderAddress = 0x8000;
}
