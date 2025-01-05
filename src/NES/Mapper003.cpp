#include "NES/Mapper003.hpp"

Mapper003::Mapper003(u8 prgNumBanks, u8 chrNumBanks, NametableArrangement ntArr) 
	: Mapper(prgNumBanks, chrNumBanks, ntArr) 
{
	mIsChrRamSelected = (chrNumBanks == 0);
}

void Mapper003::reset()
{
	mChrBankIdx = 0;
}

bool Mapper003::mapCpuWrite(u16 address, u32 &mappedAddress, u8 value)
{
	// Unreferenced argument bypass
	(void)mappedAddress;

	// Bank select register
	if (0x8000 <= address)
		mChrBankIdx = value;

	return false;
}

bool Mapper003::mapCpuRead(u16 address, u32 &mappedAddress)
{
	if (0x8000 <= address)
	{
		// Map address & handle mirroring
		mappedAddress = address & (mPrgNumBanks == 2 ? 0x7FFF : 0x3FFF);

		// The CPU accesses cartridge ROM
		return true;
	}

	// The address is not targetting the cartridge
	return false;
}

bool Mapper003::mapPpuWrite(u16 address, u32 &mappedAddress, u16 ppuCycleCount)
{
	(void)ppuCycleCount;
	return mapPpuAddress(address, mappedAddress);
}

bool Mapper003::mapPpuRead(u16 address, u32 &mappedAddress, u16 ppuCycleCount)
{
	(void)ppuCycleCount;
	return mapPpuAddress(address, mappedAddress);
}

bool Mapper003::mapPpuAddress(u16 address, u32 &mappedAddress)
{
	if (address <= 0x1FFF)
	{
		u32 maskedBank = mChrNumBanks > 4 ? 
		                 mChrBankIdx :
					     mChrBankIdx & 0x03;
		mappedAddress = (maskedBank << 13) | address;
		return true;
	}

	// The address is not targetting the cartridge
	return false;
}
