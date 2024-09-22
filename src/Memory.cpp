#include "Memory.hpp"

void Memory::initialise()
{
	// Set the whole memory to 0
	for (dword i = 0; i < MEM_SIZE; i++)
		data[i] = 0;
}

void Memory::writeWord(word value, word address, dword& cycles)
{
	// Little endian => write LSB first
	data[address] = value & 0x00FF;
	data[address + 1] = (value & 0xFF00) >> 8;

	// 1 cycle per write
	cycles -= 2;
}
