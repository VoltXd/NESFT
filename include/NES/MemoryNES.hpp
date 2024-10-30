#pragma once

#include <string>
#include "NES/Config.hpp"
#include "NES/Cartridge.hpp"

/// @brief Memory class for 6502 CPU Tests (RAM is 64 KB, while NES CPU RAM is 2 KB)
class MemoryNES
{
public:
	MemoryNES(const std::string& romFilename);
	void reset();

	u8 cpuRead(u16 address);
	void cpuWrite(u16 address, u8 value);
	
	u8 ppuRead(u16 address);
	void ppuWrite(u16 address, u8 value);

private:
	// CPU
	static constexpr u32 CPU_RAM_SIZE = 0x0800; // 2 kB
	u8 mCpuRam[CPU_RAM_SIZE];

	// PPU
	static constexpr u32 PPU_VRAM_SIZE = 0x0800; // 2 kB
	static constexpr u32 PPU_PALETTE_RAM_SIZE = 0x0020; // 32 B
	u8 mPpuVram[PPU_VRAM_SIZE];
	u8 mPpuPaletteRam[PPU_PALETTE_RAM_SIZE];


	Cartridge mCartridge;
};
