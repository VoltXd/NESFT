#include "CPUTests.hpp"

// 
TEST_F(CPUTests, rtsWorks)
{
	// Target values
	constexpr word returnAddress = 0x425D;
	constexpr byte returnLsb = 0x5D;
	constexpr byte returnMsb = 0x42;
	constexpr sdword targetCycles = RTS.cycles;

	// Run program
	memory[TEST_MAIN_ADDRESS] = RTS.opcode;
	memory[SP_PAGE_OFFSET | 0xFE] = returnLsb;
	memory[SP_PAGE_OFFSET | 0xFF] = returnMsb;
	cpu.setSp(0xFF - 2);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getSp(), 0xFF);
	EXPECT_EQ(cpu.getPc(), returnAddress + 1);
	EXPECT_EQ(elapsedCycles, targetCycles);
}