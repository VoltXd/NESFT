#include "NES/Mapper000.hpp"

Mapper000::Mapper000(u8 prgNumBanks, u8 chrNumBanks, NametableArrangement ntArr) 
	: Mapper(prgNumBanks, chrNumBanks, ntArr) 
{
	mIsChrRamSelected = (chrNumBanks == 0);
}

void Mapper000::reset()
{
	// Nothing to do I guess...
}

bool Mapper000::mapCpuWrite(u16 address, u32 &mappedAddress, u8 value)
{
	// Only PRG-ROM -> No CPU write to cartridge
	// (unreferenced parameters bypass)
	address;
	mappedAddress;
	value;
	return false;
}

bool Mapper000::mapCpuRead(u16 address, u32 &mappedAddress)
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

bool Mapper000::mapPpuWrite(u16 address, u32 &mappedAddress)
{
	return mapPpuAddress(address, mappedAddress);
}

bool Mapper000::mapPpuRead(u16 address, u32 &mappedAddress)
{
	return mapPpuAddress(address, mappedAddress);
}

bool Mapper000::mapPpuAddress(u16 address, u32 &mappedAddress)
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
