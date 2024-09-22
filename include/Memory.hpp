#pragma once

#include "Config.hpp"

class Memory
{
public:
	void initialise();
	inline byte operator[](dword index) const { return data[index]; }
	inline byte& operator[](dword index) { return data[index]; }
	void writeWord(word value, word address, dword& cycles);

private:
	static constexpr dword MEM_SIZE = 1024 * 64;
	byte data[MEM_SIZE];
};
