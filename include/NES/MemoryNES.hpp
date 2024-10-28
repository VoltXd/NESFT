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

private:
	static constexpr u32 CPU_RAM_SIZE = 1 << 11; // 2 kB
	u8 mCpuRam[CPU_RAM_SIZE];

	Cartridge mCartridge;
};
