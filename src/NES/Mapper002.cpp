#include "NES/Mapper002.hpp"

Mapper002::Mapper002(u8 prgNumBanks, u8 chrNumBanks, NametableArrangement ntArr) 
	: Mapper(prgNumBanks, chrNumBanks, ntArr) 
{
	mIsChrRamSelected = (chrNumBanks == 0);
}

void Mapper002::reset()
{
	mPrgBankIdx = 0;
}

bool Mapper002::mapCpuWrite(u16 address, u32 &mappedAddress, u8 value)
{
	// Unreferenced argument bypass
	(void)mappedAddress;

	// Bank select register
	if (0x8000 <= address)
		mPrgBankIdx = value;

	return false;
}

bool Mapper002::mapCpuRead(u16 address, u32 &mappedAddress)
{
	// Is address targetting the cartridge ?
	if (address < 0x8000)
		return false;

	u32 offset;
	if (address < 0xC000)
	{
		// The address targets the switchable PRG ROM bank
		offset = ((u32)mPrgBankIdx << 14);
	}
	else
	{
		// The address targets the last PRG ROM bank
		const u32 LAST_BANK_OFFSET = (u32)(mPrgNumBanks - 1) << 14;
		offset = LAST_BANK_OFFSET; 
	}

	mappedAddress = offset | (address & 0x3FFF);

	// The CPU accesses cartridge ROM
	return true;
}

bool Mapper002::mapPpuWrite(u16 address, u32 &mappedAddress, u16 ppuCycleCount)
{
	(void)ppuCycleCount;
	return mapPpuAddress(address, mappedAddress);
}

bool Mapper002::mapPpuRead(u16 address, u32 &mappedAddress, u16 ppuCycleCount)
{
	(void)ppuCycleCount;
	return mapPpuAddress(address, mappedAddress);
}

bool Mapper002::mapPpuAddress(u16 address, u32 &mappedAddress)
{
	// No mapping required
	if (address <= 0x1FFF)
	{
		mappedAddress = address;
		return true;
	}

	// The address is not targetting the cartridge
	return false;
}
