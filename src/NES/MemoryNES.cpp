#include "NES/MemoryNES.hpp"

#include "NES/PPU.hpp"
#include "NES/APU.hpp"

#include <random>
#include <iostream>

MemoryNES::MemoryNES(const std::string &romFilename, APU& apuRef, PPU& ppuRef, Controller& controllerRef)
	: mCartridge(romFilename), mApuRef(apuRef), mPpuRef(ppuRef), mControllerRef(controllerRef)
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

	mIsOamDmaStarted = false;
	mIsCpuHalt = false;
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
		value = mPpuRef.readRegister(*this, address & 0x2007);
	}
	else if (0x4000 <= address && address < 0x4018)
	{
		// APU & IO Registers
		if (address == APU_STATUS_CPU_ADDR)
			 // APU status
			value = mApuRef.readRegister(address);

		if (address == CONTROLLER_1_STATE_ADDR)
			// Controller 1
			value = mControllerRef.getStateBitAndShift();
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
	else if ((0x2000 <= address && address < 0x4000))
	{
		// PPU Registers
		mPpuRef.writeRegister(*this, address & 0x2007, value);
	}
	else if (0x4000 <= address && address < 0x4018)
	{
		// APU & IO Registers
		if ((0x4000 <= address && address <= 0x4013) || 
		    address == APU_STATUS_CPU_ADDR           || 
			address == APU_FRAME_COUNTER_CPU_ADDR)
			// APU
			mApuRef.writeRegister(address, value);

		else if (address == OAMDMA_CPU_ADDR)
			// OAM DMA
			startOamDma(value);
			
		else if (address == CONTROLLER_STROBE_ADDR)
			// Controller 1
			mControllerRef.setStrobe(value);
	}
	else if (0x4018 <= address && address < 0x4020)
	{
		// APU & IO test registers
	}
}

u8 MemoryNES::ppuRead(u16 address)
{
	address &= 0x3FFF;
	u16 mappedNtAddress;

	u8 value = 0;
	bool isInCartridgeMemory = mCartridge.readChr(address, value, mappedNtAddress);
	if (isInCartridgeMemory)
		return value;

	if (0x2000 <= address && address < 0x3F00)
	{
		value = mPpuVram[mappedNtAddress];
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
	u16 mappedNtAddress = 0;
	
	bool isInCartridgeMemory = mCartridge.writeChr(address, value, mappedNtAddress);
	if (isInCartridgeMemory)
		return;

	if (0x2000 <= address && address < 0x3F00)
	{
		mPpuVram[mappedNtAddress] = value;
	}
	else if (0x3F00 <= address && address < 0x4000)
	{
		u16 paletteRamAddress = address;
		paletteRamAddress &= ((paletteRamAddress & 0x0003) == 0) ? 0x000F : 0x001F;
		mPpuPaletteRam[paletteRamAddress] = value;
	}
}

s32 MemoryNES::executeOamDma(bool isGetCycle)
{
	s32 elapsedCycles = 1;

	if (!mIsCpuHalt)
	{
		// Do halt cycle
		mIsCpuHalt = true;
	}
	else if ((mOamDmaIdx == 0) && !isGetCycle)
		// Do alignment cycle (optionnal)
		elapsedCycles = 1;
	else if ((mOamDmaIdx < 256) && isGetCycle)
	{
		// Read from RAM to DMA
		u16 cpuAddress = (u16)(mOamDma << 8) | mOamDmaIdx;
		mOamDmaBuffer = cpuRead(cpuAddress);

		mOamDmaIdx++;
	}
	else if ((mOamDmaIdx <= 256) && !isGetCycle)
	{
		// Write from DMA to OAM
		cpuWrite(OAMDATA_CPU_ADDR, mOamDmaBuffer);

		if (mOamDmaIdx == 256)
		{
			// DMA Complete
			mIsOamDmaStarted = false;
			mIsCpuHalt = false;
		}
	}

    return elapsedCycles;
}

void MemoryNES::startOamDma(u8 pageAddress)
{
	mOamDma = pageAddress;
	mOamDmaIdx = 0;
	mIsOamDmaStarted = true;
}