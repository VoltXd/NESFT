#include "CPUTests.hpp"

// 
TEST_F(CPUTests, rtsWorks)
{
	// Target values
	constexpr u16 returnAddress = 0x425D;
	constexpr u8 returnLsb = 0x5D;
	constexpr u8 returnMsb = 0x42;
	constexpr s32 targetCycles = RTS.cycles;

	// Run program
	memory[TEST_MAIN_ADDRESS] = RTS.opcode;
	memory[SP_PAGE_OFFSET | 0xFE] = returnLsb;
	memory[SP_PAGE_OFFSET | 0xFF] = returnMsb;
	cpu.setSp(0xFF - 2);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getSp(), 0xFF);
	EXPECT_EQ(cpu.getPc(), returnAddress + 1);
	EXPECT_EQ(elapsedCycles, targetCycles);
}