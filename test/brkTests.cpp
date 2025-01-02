#include "CPUTests.hpp"

// 
TEST_F(CPUTests, brkWorks)
{
	// Target values
	constexpr u16 targetAddress = 0x425D;
	constexpr u8 targetLsb = 0x5D;
	constexpr u8 targetMsb = 0x42;
	constexpr s32 targetCycles = BRK.cycles;

	// Run program
	cpu.setC(1);
	cpu.setV(1);
	cpu.setN(1);
	memory[TEST_MAIN_ADDRESS] = BRK.opcode;
	memory[IRQ_VECTOR_LSB] = targetLsb;
	memory[IRQ_VECTOR_MSB] = targetMsb;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getSp(), 0xFF - 5);
	EXPECT_EQ(cpu.getPc(), targetAddress);
	EXPECT_EQ(memory[SP_PAGE_OFFSET | 0xFB], 0b1111'0101);
	EXPECT_EQ(memory[SP_PAGE_OFFSET | 0xFC], (TEST_MAIN_ADDRESS + 2) & 0x00FF);
	EXPECT_EQ(memory[SP_PAGE_OFFSET | 0xFD], ((TEST_MAIN_ADDRESS + 2) & 0xFF00) >> 8);
	EXPECT_EQ(elapsedCycles, targetCycles);
}