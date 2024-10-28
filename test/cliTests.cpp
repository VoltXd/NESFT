#include "CPUTests.hpp"

// ************** Clear ************** //
TEST_F(CPUTests, cliClear)
{
	// Target values
	constexpr s32 targetCycles = CLI.cycles;

	// Run program
	cpu.setI(1);
	memory[TEST_MAIN_ADDRESS] = CLI.opcode;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getI(), 0);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ************** No change ************** //
TEST_F(CPUTests, cliClearUnchange)
{
	// Target values
	constexpr s32 targetCycles = CLI.cycles;

	// Run program
	cpu.setI(0);
	memory[TEST_MAIN_ADDRESS] = CLI.opcode;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getI(), 0);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
