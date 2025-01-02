#pragma once

#include "NES/Config.hpp"

/// @brief Memory class for 6502 CPU Tests (RAM is 64 KB, while NES CPU RAM is 2 KB)
class Memory6502
{
public:
	void reset();
	inline u8 operator[](u16 index) const { return data[index]; }
	inline u8& operator[](u16 index) { return data[index]; }
	
	inline u8 cpuRead(u16 address) const { return data[address]; }
	inline void cpuWrite(u16 address, u8 value) { data[address] = value; }

private:
	static constexpr s32 MEM_SIZE = 1024 * 64;
	u8 data[MEM_SIZE];
};
