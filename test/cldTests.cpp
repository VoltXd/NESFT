#include "CPUTests.hpp"

// ************** Clear ************** //
TEST_F(CPUTests, cldClear)
{
	// Target values
	constexpr sdword targetCycles = CLD.cycles;

	// Run program
	cpu.setD(1);
	memory[PC_RESET] = CLD.opcode;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getD(), 0);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ************** No change ************** //
TEST_F(CPUTests, cldClearUnchange)
{
	// Target values
	constexpr sdword targetCycles = CLD.cycles;

	// Run program
	cpu.setD(0);
	memory[PC_RESET] = CLD.opcode;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getD(), 0);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
