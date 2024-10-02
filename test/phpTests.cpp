#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromPHP(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getZ(), cpuInitialState.getZ());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
	EXPECT_EQ(cpu.getN(), cpuInitialState.getN());
}

TEST_F(CPUTests, phpWorks)
{
	// Target values
	cpu.setC(1);
	cpu.setN(1);
	const CPU cpuInitialState = cpu;
	constexpr sdword targetCycles = PHP.cycles;

	// Push Processor status
	memory[PC_RESET] = PHP.opcode;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	// NVUB'DIZC = 0b1011'0001
	EXPECT_EQ(memory[SP_PAGE_OFFSET + cpu.getSp() + 1], 0b1011'0001);
	verifyUnmodifiedStatusFlagsFromPHP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
