#include "NES/MemoryNES.hpp"

#include "NES/PPU.hpp"

#include <random>

MemoryNES::MemoryNES(const std::string &romFilename, PPU& ppuRef)
	: mCartridge(romFilename), mPpuRef(ppuRef)
{
}

void MemoryNES::reset()
{
	// Set the whole memory to 0
	for (u32 i = 0; i < CPU_RAM_SIZE; i++)
		mCpuRam[i] = 0;

	for (u32 i = 0; i < PPU_PALETTE_RAM_SIZE; i++)
		mPpuPaletteRam[i] = rand() % 0x40;

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
		value = mPpuRef.readRegister(*this, address);
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
		mPpuRef.writeRegister(*this, address, value);
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

u8 MemoryNES::ppuRead(u16 address)
{
	address &= 0x3FFF;

	u8 value = 0;
	bool isInCartridgeMemory = mCartridge.readChr(address, value);
	if (isInCartridgeMemory)
		return value;

	if (0x2000 <= address && address < 0x3F00)
	{
		NametableArrangement ntArr = mCartridge.getNtArrangement();
		// u16 vramAddress = address & (ntArr == NametableArrangement::VERT ? 0b : );
		u16 vramAddress;
		if (ntArr == NametableArrangement::VERT)
			vramAddress = (address & 0b0000'0011'1111'1111) | ((address >> 1) & 0b0000'0100'0000'0000);
		else
			vramAddress = address & 0b0000'0111'1111'1111;
	
		value = mPpuVram[vramAddress];
	}
	else if (0x3F00 <= address && address < 0x4000)
	{
		u16 paletteRamAddress = address;
		paletteRamAddress &= ((paletteRamAddress & 0x0003) == 0) ? 0x000F : 0x001F;
		value = mPpuPaletteRam[paletteRamAddress];
	}

    return value;
}

void MemoryNES::ppuWrite(u16 address, u8 value)
{
	address &= 0x3FFF;
	
	bool isInCartridgeMemory = mCartridge.writeChr(address, value);
	if (isInCartridgeMemory)
		return;

	if (0x2000 <= address && address < 0x3F00)
	{
		NametableArrangement ntArr = mCartridge.getNtArrangement();
		// u16 vramAddress = address & (ntArr == NametableArrangement::VERT ? 0b : );
		u16 vramAddress;
		if (ntArr == NametableArrangement::VERT)
			vramAddress = (address & 0b0000'0011'1111'1111) | ((address >> 1) & 0b0000'0100'0000'0000);
		else
			vramAddress = address & 0b0000'0111'1111'1111;
	
		mPpuVram[vramAddress] = value;
	}
	else if (0x3F00 <= address && address < 0x4000)
	{
		u16 paletteRamAddress = address;
		paletteRamAddress &= ((paletteRamAddress & 0x0003) == 0) ? 0x000F : 0x001F;
		mPpuPaletteRam[paletteRamAddress] = value;
	}
}