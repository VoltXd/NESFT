#include "CPUTests.hpp"

// 
TEST_F(CPUTests, jsrWorks)
{
	// Target values
	constexpr u16 targetAddress = 0x425D;
	constexpr u8 targetLsb = 0x5D;
	constexpr u8 targetMsb = 0x42;
	constexpr s32 targetCycles = JSR.cycles;

	// Run program
	memory[TEST_MAIN_ADDRESS] = JSR.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = targetLsb;
	memory[TEST_MAIN_ADDRESS + 2] = targetMsb;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getSp(), 0xFF - 4);
	EXPECT_EQ(cpu.getPc(), targetAddress);
	EXPECT_EQ(memory[SP_PAGE_OFFSET | 0xFC], (TEST_MAIN_ADDRESS + 2) & 0x00FF);
	EXPECT_EQ(memory[SP_PAGE_OFFSET | 0xFD], ((TEST_MAIN_ADDRESS + 2) & 0xFF00) >> 8);
	EXPECT_EQ(elapsedCycles, targetCycles);
}