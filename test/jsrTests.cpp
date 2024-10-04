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
	memory[TEST_MAIN_ADDRESS] = JSR.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = targetLsb;
	memory[TEST_MAIN_ADDRESS + 2] = targetMsb;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getSp(), 0xFF - 5);
	EXPECT_EQ(cpu.getPc(), targetAddress);
	EXPECT_EQ(memory[SP_PAGE_OFFSET | 0xFB], (TEST_MAIN_ADDRESS + 2) & 0x00FF);
	EXPECT_EQ(memory[SP_PAGE_OFFSET | 0xFC], ((TEST_MAIN_ADDRESS + 2) & 0xFF00) >> 8);
	EXPECT_EQ(elapsedCycles, targetCycles);
}