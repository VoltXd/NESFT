#include "Memory.hpp"

void Memory::initialise()
{
	// Set the whole memory to 0
	for (sdword i = 0; i < MEM_SIZE; i++)
		data[i] = 0;
}

void Memory::writeWord(word value, word address, sdword& cycles)
{
	// 1 cycle per write
	data[address] = value & 0x00FF;
	cycles--;

	data[address + 1] = (value & 0xFF00) >> 8;
	cycles--;
}
