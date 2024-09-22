#include "Emulator.hpp"

#include <cstdlib>
#include "CPU.hpp"
#include "Memory.hpp"

int Emulator::run()
{
	// Instantiations
	CPU cpu;
	Memory memory;

	// *************** CPU Emulation *************** //
	// Reset
	cpu.reset(memory);

	// Fill memory with fake program
	memory[0xFFFC] = INS_LDA_IMM;
	memory[0xFFFC + 1] = 0x42;

	// Execute
	cpu.execute(2, memory);

	return EXIT_SUCCESS;
}
