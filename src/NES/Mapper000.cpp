#include "NES/Mapper000.hpp"

void Mapper000::reset()
{
	// Nothing to do I guess...
}

bool Mapper000::mapCpuWrite(word address, uint32_t &mappedAddress)
{
	// ROM -> No CPU write to cartridge
	// Implemented as CPU read to remove unused-parameter error
	if (0x8000 <= address)
	{
		mappedAddress = address & (mPrgNumBanks == 1 ? 0x7FFF : 0x3FFF);

		// The CPU accesses cartridge ROM
		return true;
	}

	// The address is not targetting the cartridge
	return false;
}

bool Mapper000::mapCpuRead(word address, uint32_t &mappedAddress)
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

bool Mapper000::mapPpuWrite(word address, uint32_t &mappedAddress)
{
	// No mapping required
	if (address <= 0x1FFF)
	{
		if (mChrNumBanks == 0)
		{
			mappedAddress = address;
			return true;
		}
	}

	// The address is not targetting the cartridge
	return false;
}

bool Mapper000::mapPpuRead(word address, uint32_t &mappedAddress)
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
