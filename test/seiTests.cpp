#include "CPUTests.hpp"

// ************** No change ************** //
TEST_F(CPUTests, seiSetUnchange)
{
	// Target values
	constexpr sdword targetCycles = SEI.cycles;

	// Run program
	cpu.setI(1);
	memory[PC_RESET] = SEI.opcode;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getI(), 1);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ************** Set ************** //
TEST_F(CPUTests, seiSets)
{
	// Target values
	constexpr sdword targetCycles = SEI.cycles;

	// Run program
	cpu.setI(0);
	memory[PC_RESET] = SEI.opcode;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getI(), 1);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
