#include "CPUTests.hpp"

// ************** No change ************** //
TEST_F(CPUTests, secSetUnchange)
{
	// Target values
	constexpr s32 targetCycles = SEC.cycles;

	// Run program
	cpu.setC(1);
	memory[TEST_MAIN_ADDRESS] = SEC.opcode;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getC(), 1);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ************** Set ************** //
TEST_F(CPUTests, secSet)
{
	// Target values
	constexpr s32 targetCycles = SEC.cycles;

	// Run program
	cpu.setC(0);
	memory[TEST_MAIN_ADDRESS] = SEC.opcode;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getC(), 1);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
