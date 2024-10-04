#include "CPUTests.hpp"

// ************** No change ************** //
TEST_F(CPUTests, sedSetUnchange)
{
	// Target values
	constexpr sdword targetCycles = SED.cycles;

	// Run program
	cpu.setD(1);
	memory[TEST_MAIN_ADDRESS] = SED.opcode;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getD(), 1);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ************** No change ************** //
TEST_F(CPUTests, sedSets)
{
	// Target values
	constexpr sdword targetCycles = SED.cycles;

	// Run program
	cpu.setD(0);
	memory[TEST_MAIN_ADDRESS] = SED.opcode;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getD(), 1);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
