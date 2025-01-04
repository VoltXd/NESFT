#include "NES/Mapper004.hpp"
#include "NES/Toolbox.hpp"
#include <iomanip>

Mapper004::Mapper004(u8 prgNumBanks, u8 chrNumBanks, NametableArrangement ntArr)
	: Mapper(prgNumBanks, chrNumBanks, ntArr)
{
	mIsChrRamSelected = (chrNumBanks == 0);
}

void Mapper004::reset()
{
	mBankSelected = 0;
	mPrgBankMode = 0;
	mChrBankMode = 0;

	mPreviousA12 = 0;
	mIsIrqEnabled = false;
	mIsIrqReloadSet = false;
	mIsIrqSignalSet = false;
	mIrqCounter.reset();
	mPreviousCounter = 0;
	mPreviousPpuCycle = 0;
	mM2CycleOffset = 0;
}

bool Mapper004::mapCpuWrite(u16 address, u32 &mappedAddress, u8 value)
{
	// Is the cartridge targeted ?
	if (address < 0x6000)
		return false;
	
	// Is the PRG-RAM targeted ?
	else if (address < 0x8000)
	{
		// Using iNES 1.0 => Assume Battery Backed SRAM is 8 kB
		mappedAddress = address & 0x1FFF;
		return true;
	}

	// The mapper registers are targeted
	processRegisterWrite(address, value);
	return false;
}

bool Mapper004::mapCpuRead(u16 address, u32 &mappedAddress)
{
	// Is the cartridge targeted ?
	if (address < 0x6000)
		return false;

	// Is PRG-RAM targeted ? 
	else if (address < 0x8000)
	{
		// PRG-RAM
		mappedAddress = address & 0x1FFF;
		mIsPrgRamRead = true;
		return true;
	}

	// PRG-ROM is targeted
	switch (mPrgBankMode)
	{
		case 0:
			if (address < 0xA000)
			{
				// PRG bank R6
				mappedAddress = ((u32)(mBankRegisters[6] & 0b0011'1111) << 13) | (0x1FFF & address);
			}
			else if (address < 0xC000)
			{
				// PRG bank R7
				mappedAddress = ((u32)(mBankRegisters[7] & 0b0011'1111) << 13) | (0x1FFF & address);
			}
			else if (address < 0xE000)
			{
				// Fixed to second-last bank
				const u32 SECOND_LAST_BANK_OFFSET = ((u32)(mPrgNumBanks - 1) << 14);
				mappedAddress = SECOND_LAST_BANK_OFFSET | (0x1FFF & address);
			}
			else
			{
				// Fixed to last bank
				const u32 LAST_BANK_OFFSET = ((u32)(mPrgNumBanks - 1) << 14) | 0x2000;
				mappedAddress = LAST_BANK_OFFSET | (0x1FFF & address);
			}
			break;

		case 1:
			if (address < 0xA000)
			{
				// Fixed to second-last bank
				const u32 SECOND_LAST_BANK_OFFSET = ((u32)(mPrgNumBanks - 1) << 14);
				mappedAddress = SECOND_LAST_BANK_OFFSET | (0x1FFF & address);
			}
			else if (address < 0xC000)
			{
				// PRG bank R7
				mappedAddress = ((u32)(mBankRegisters[7] & 0b0011'1111) << 13) | (0x1FFF & address);
			}
			else if (address < 0xE000)
			{
				// PRG bank R6
				mappedAddress = ((u32)(mBankRegisters[6] & 0b0011'1111) << 13) | (0x1FFF & address);
			}
			else
			{
				// Fixed to last bank
				const u32 LAST_BANK_OFFSET = ((u32)(mPrgNumBanks - 1) << 14) | 0x2000;
				mappedAddress = LAST_BANK_OFFSET | (0x1FFF & address);
			}
			break;
	}

	mIsPrgRamRead = false;
	return true;
}

bool Mapper004::mapPpuWrite(u16 address, u32 &mappedAddress, u16 ppuCycleCount)
{
	return mapPpuAddress(address, mappedAddress, ppuCycleCount);
}

bool Mapper004::mapPpuRead(u16 address, u32 &mappedAddress, u16 ppuCycleCount)
{
	return mapPpuAddress(address, mappedAddress, ppuCycleCount);
}

void Mapper004::processRegisterWrite(u16 address, u8 value)
{
	constexpr u16 ADDRESS_MASK    = 0x6001;
	constexpr u16 BANK_SELECT     = 0x0000;
	constexpr u16 BANK_DATA       = 0x0001;
	constexpr u16 MIRRORING       = 0x2000;
	constexpr u16 PRG_RAM_PROTECT = 0x2001;
	constexpr u16 IRQ_LATCH       = 0x4000;
	constexpr u16 IRQ_RELOAD      = 0x4001;
	constexpr u16 IRQ_DISABLE     = 0x6000;
	constexpr u16 IRQ_ENABLE      = 0x6001;

	switch (address & ADDRESS_MASK)
	{
		case BANK_SELECT:
			mBankSelected = value & 0b0000'0111;
			mPrgBankMode = (value & 0b0100'0000) >> 6;
			mChrBankMode = (value & 0b1000'0000) >> 7;
			break;
			
		case BANK_DATA:
			mBankRegisters[mBankSelected] = value;
			break;
			
		case MIRRORING:
			if (mNtArrangement != NametableArrangement::FOUR_SCREEN)
				mNtArrangement = (value & 0x01) == 0 ?
				                 NametableArrangement::HOR :
								 NametableArrangement::VERT;
			break;
			
		case PRG_RAM_PROTECT:
			// Do nothing for MMC3
			// TODO: MMC6
			break;
			
		case IRQ_LATCH:
			mIrqCounter.loadPeriod(value);
			mPreviousCounter = value;
			break;
			
		case IRQ_RELOAD:
			mIsIrqReloadSet = true;
			break;
			
		case IRQ_DISABLE:
			mIsIrqEnabled = false;
			mIsIrqSignalSet = false;
			break;
			
		case IRQ_ENABLE:
			mIsIrqEnabled = true;
			break;
	}
}

bool Mapper004::mapPpuAddress(u16 address, u32 &mappedAddress, u16 ppuCycleCount)
{
	processIrqCounter(address, ppuCycleCount);

	// Mapping for CHR-ROM/RAM (pattern tables)
	if (address <= 0x1FFF)
	{
		// CHR A12 inversion
		address ^= mChrBankMode << 12;

		// Map address
		if (address < 0x0800)
		{
			u32 offset = (mBankRegisters[0] & 0b1111'1110) << 10;
			mappedAddress = offset | (address & 0x7FF);
		}
		else if (address < 0x1000)
		{
			u32 offset = (mBankRegisters[1] & 0b1111'1110) << 10;
			mappedAddress = offset | (address & 0x7FF);
		}
		else if (address < 0x1400)
		{
			u32 offset = (mBankRegisters[2] & 0b1111'1111) << 10;
			mappedAddress = offset | (address & 0x3FF);
		}
		else if (address < 0x1800)
		{
			u32 offset = (mBankRegisters[3] & 0b1111'1111) << 10;
			mappedAddress = offset | (address & 0x3FF);
		}
		else if (address < 0x1C00)
		{
			u32 offset = (mBankRegisters[4] & 0b1111'1111) << 10;
			mappedAddress = offset | (address & 0x3FF);
		}
		else
		{
			u32 offset = (mBankRegisters[5] & 0b1111'1111) << 10;
			mappedAddress = offset | (address & 0x3FF);
		}

		return true;
	}

	// The remainder of the memory is in the PPU
	return false;
}

void Mapper004::processIrqCounter(u16 address, u16 ppuCycleCount)
{
	// Check for a toggle in PPU A12
	u8 currentA12 = (address & 0x1000) >> 12;

	bool shouldClock = false;
	if ((mPreviousA12 == 0) && (currentA12 == 1))
	{
		// Rising edge
		u8 filterThreshold = 9 + mM2CycleOffset;

		// M2 filtering
		shouldClock = mPpuCycleElapsed >= filterThreshold;
		if (shouldClock)
			clockIrqCounter();
	}
	else if ((mPreviousA12 == 1) && (currentA12 == 0))
	{
		// Falling edge
		mPpuCycleElapsed = 1;
	}
	else if (currentA12 == 0)
	{
		mPpuCycleElapsed += (ppuCycleCount >= mPreviousPpuCycle) ? 
							(ppuCycleCount - mPreviousPpuCycle) :
							(341 + ppuCycleCount - mPreviousPpuCycle);
	}

	if (ppuCycleCount >= mPreviousPpuCycle)
		mM2CycleOffset = (mM2CycleOffset + 2) % 3;

	// Log MMC3 IRQ
	irqLog(ppuCycleCount, address, shouldClock);
	
	mPreviousPpuCycle = ppuCycleCount;
	mPreviousA12 = currentA12;
}

void Mapper004::clockIrqCounter()
{
	mPreviousCounter = mIrqCounter.getCounter();

	if (mIsIrqReloadSet)
		mIrqCounter.reloadCounter();
	else
		mIrqCounter.countDown();		
	
	u16 currentCounter = mIrqCounter.getCounter();
	if (((mPreviousCounter != 0) || mIsIrqReloadSet) && currentCounter == 0 && mIsIrqEnabled)
		mIsIrqSignalSet = true;
	
	mIsIrqReloadSet = false;
}

void Mapper004::irqLog(u16 cycle, u16 address, bool hasClocked)
{
	if (!gIsTraceLogMMC3IrqEnabled)
		return;

	// Cycle, Addr, Clocked ?, timer
	std::stringstream mmc3IrqTraceStream;
	mmc3IrqTraceStream << "Cycle: " << std::setw(3) << cycle 
                       << ", Address: $" << std::uppercase << std::hex << std::setw(4) << address
                       << ", Clocked: " << std::boolalpha << std::setw(5) << hasClocked 
                       << ", Period: " << std::setw(3) << mIrqCounter.getPeriod() 
                       << ", Counter: " << std::setw(3) << mIrqCounter.getCounter() << "\n";
	traceLog(mmc3IrqTraceStream.str());
}