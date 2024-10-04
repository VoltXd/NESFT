#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromRTI(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getZ(), cpuInitialState.getZ());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
}

// 
TEST_F(CPUTests, rtiWorks)
{
	// Target values
	CPU cpuInitialState = cpu;
	constexpr word returnAddress = 0x425D;
	constexpr byte returnLsb = 0x5D;
	constexpr byte returnMsb = 0x42;
	constexpr sdword targetCycles = RTI.cycles;

	// Run program
	memory[TEST_MAIN_ADDRESS] = RTI.opcode;
	memory[SP_PAGE_OFFSET | 0xFA] = 0b1111'0101;
	memory[SP_PAGE_OFFSET | 0xFB] = returnLsb;
	memory[SP_PAGE_OFFSET | 0xFC] = returnMsb;
	cpu.setSp(0xFF - 6);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	verifyUnmodifiedStatusFlagsFromRTI(cpu, cpuInitialState);
	EXPECT_TRUE(cpu.getC());
	EXPECT_TRUE(cpu.getB());
	EXPECT_TRUE(cpu.getV());
	EXPECT_TRUE(cpu.getN());
	EXPECT_EQ(cpu.getSp(), 0xFC);
	EXPECT_EQ(cpu.getPc(), returnAddress);
	EXPECT_EQ(elapsedCycles, targetCycles);
}