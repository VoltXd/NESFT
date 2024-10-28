#include "CPUTests.hpp"

// ************** Clear ************** //
TEST_F(CPUTests, cldClear)
{
	// Target values
	constexpr s32 targetCycles = CLD.cycles;

	// Run program
	cpu.setD(1);
	memory[TEST_MAIN_ADDRESS] = CLD.opcode;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getD(), 0);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ************** No change ************** //
TEST_F(CPUTests, cldClearUnchange)
{
	// Target values
	constexpr s32 targetCycles = CLD.cycles;

	// Run program
	cpu.setD(0);
	memory[TEST_MAIN_ADDRESS] = CLD.opcode;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getD(), 0);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
