#include "CPUTests.hpp"

// 
TEST_F(CPUTests, brkWorks)
{
	// Target values
	constexpr word targetAddress = 0x425D;
	constexpr byte targetLsb = 0x5D;
	constexpr byte targetMsb = 0x42;
	constexpr sdword targetCycles = BRK.cycles;

	// Run program
	cpu.setC(1);
	cpu.setV(1);
	cpu.setN(1);
	memory[TEST_MAIN_ADDRESS] = BRK.opcode;
	memory[IRQ_VECTOR_LSB] = targetLsb;
	memory[IRQ_VECTOR_MSB] = targetMsb;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getSp(), 0xFF - 6);
	EXPECT_EQ(cpu.getPc(), targetAddress);
	EXPECT_EQ(memory[SP_PAGE_OFFSET | 0xFA], 0b1111'0101);
	EXPECT_EQ(memory[SP_PAGE_OFFSET | 0xFB], (TEST_MAIN_ADDRESS + 2) & 0x00FF);
	EXPECT_EQ(memory[SP_PAGE_OFFSET | 0xFC], ((TEST_MAIN_ADDRESS + 2) & 0xFF00) >> 8);
	EXPECT_EQ(elapsedCycles, targetCycles);
}