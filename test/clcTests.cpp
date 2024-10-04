#include "CPUTests.hpp"

// ************** Clear ************** //
TEST_F(CPUTests, clcClear)
{
	// Target values
	constexpr sdword targetCycles = CLC.cycles;

	// Run program
	cpu.setC(1);
	memory[TEST_MAIN_ADDRESS] = CLC.opcode;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getC(), 0);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ************** No change ************** //
TEST_F(CPUTests, clcClearUnchange)
{
	// Target values
	constexpr sdword targetCycles = CLC.cycles;

	// Run program
	cpu.setC(0);
	memory[TEST_MAIN_ADDRESS] = CLC.opcode;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getC(), 0);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
