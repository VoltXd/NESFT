#include "NES/MemoryNES.hpp"

MemoryNES::MemoryNES(const std::string &romFilename)
	: mCartridge(romFilename)
{
}

void MemoryNES::reset()
{
	// Set the whole memory to 0
	for (u32 i = 0; i < CPU_RAM_SIZE; i++)
		mCpuRam[i] = 0;

	mCartridge.reset();
}

u8 MemoryNES::cpuRead(u16 address)
{
	u8 value = 0;
	bool isInCartridgeMemory= mCartridge.readPrg(address, value);
	if (isInCartridgeMemory)
		return value;
	
	if (address < 0x2000)
	{
		// CPU RAM, mirrored 4 times
		value = mCpuRam[address & 0x07FF];
	}
	else if (0x2000 <= address && address < 0x4000)
	{
		// PPU Registers
	}
	else if (0x4000 <= address && address < 0x4018)
	{
		// APU & IO Registers
	}
	else if (0x4018 <= address && address < 0x4020)
	{
		// APU & IO test registers
	}

	return value;
}

void MemoryNES::cpuWrite(u16 address, u8 value)
{
	// 0x4020 - 0xFFFF
	bool isInCartridgeMemory= mCartridge.writePrg(address, value);
	if (isInCartridgeMemory)
		return;
	
	if (address < 0x2000)
	{
		// CPU RAM, mirrored 4 times
		mCpuRam[address & 0x07FF] = value;
	}
	else if (0x2000 <= address && address < 0x4000)
	{
		// PPU Registers
	}
	else if (0x4000 <= address && address < 0x4018)
	{
		// APU & IO Registers
	}
	else if (0x4018 <= address && address < 0x4020)
	{
		// APU & IO test registers
	}
}