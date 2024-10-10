#include "Memory6502.hpp"

void Memory6502::reset()
{
	// Set the whole memory to 0
	for (sdword i = 0; i < MEM_SIZE; i++)
		data[i] = 0;
}
