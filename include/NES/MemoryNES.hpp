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
	byte cpuRead(word address);
	void cpuWrite(word address, byte value);

private:
	static constexpr dword CPU_RAM_SIZE = 1 << 11; // 2 kB
	byte mCpuRam[CPU_RAM_SIZE];

	Cartridge mCartridge;
};
