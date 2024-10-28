#pragma once

#include "NES/Config.hpp"

/// @brief Memory class for 6502 CPU Tests (RAM is 64 KB, while NES CPU RAM is 2 KB)
class Memory6502
{
public:
	void reset();
	inline byte operator[](word index) const { return data[index]; }
	inline byte& operator[](word index) { return data[index]; }

private:
	static constexpr sdword MEM_SIZE = 1024 * 64;
	byte data[MEM_SIZE];
};
