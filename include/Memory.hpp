#pragma once

#include "Config.hpp"

class Memory
{
public:
	void initialise();
	inline byte operator[](word index) const { return data[index]; }
	inline byte& operator[](word index) { return data[index]; }
	void writeWord(word value, word address, sdword& cycles);

private:
	static constexpr sdword MEM_SIZE = 1024 * 64;
	byte data[MEM_SIZE];
};
