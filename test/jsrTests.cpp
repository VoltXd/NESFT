#include "CPUTests.hpp"

// 
TEST_F(CPUTests, jsrWorks)
{
	// Target values
	constexpr word targetAddress = 0x425D;
	constexpr byte targetLsb = 0x5D;
	constexpr byte targetMsb = 0x42;
	constexpr sdword targetCycles = JSR.cycles;

	// Run program
	memory[PC_RESET] = JSR.opcode;
	memory[PC_RESET + 1] = targetLsb;
	memory[PC_RESET + 2] = targetMsb;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getSp(), 0xFF - 2);
	EXPECT_EQ(cpu.getPc(), targetAddress);
	EXPECT_EQ(memory[SP_PAGE_OFFSET | 0xFF], ((PC_RESET + 2) & 0xFF00) >> 8);
	EXPECT_EQ(memory[SP_PAGE_OFFSET | 0xFE], (PC_RESET + 2) & 0x00FF);
	EXPECT_EQ(elapsedCycles, targetCycles);
}