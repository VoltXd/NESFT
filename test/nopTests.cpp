#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromNOP(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getZ(), cpuInitialState.getZ());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
	EXPECT_EQ(cpu.getN(), cpuInitialState.getN());
	
	EXPECT_EQ(cpu.getA(), cpuInitialState.getA());
	EXPECT_EQ(cpu.getX(), cpuInitialState.getX());
	EXPECT_EQ(cpu.getY(), cpuInitialState.getY());
	
	EXPECT_EQ(cpu.getSp(), cpuInitialState.getSp());
}
TEST_F(CPUTests, nopWorks)
{
	// Target values
	CPU cpuInitialState = cpu;
	constexpr sdword targetCycles = NOP.cycles;

	// Do nothing
	memory[TEST_MAIN_ADDRESS] = NOP.opcode;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), TEST_MAIN_ADDRESS + 1);
	verifyUnmodifiedStatusFlagsFromNOP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}