#include "CPUTests.hpp"

// ************** Clear ************** //
TEST_F(CPUTests, clvClear)
{
	// Target values
	constexpr s32 targetCycles = CLV.cycles;

	// Run program
	cpu.setV(1);
	memory[TEST_MAIN_ADDRESS] = CLV.opcode;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getV(), 0);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ************** No change ************** //
TEST_F(CPUTests, clvClearUnchange)
{
	// Target values
	constexpr s32 targetCycles = CLV.cycles;

	// Run program
	cpu.setV(0);
	memory[TEST_MAIN_ADDRESS] = CLV.opcode;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getV(), 0);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
