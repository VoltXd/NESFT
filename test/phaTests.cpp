#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromPHA(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getZ(), cpuInitialState.getZ());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
	EXPECT_EQ(cpu.getN(), cpuInitialState.getN());
}

TEST_F(CPUTests, phaWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = PHA.cycles;

	// Push A
	memory[TEST_MAIN_ADDRESS] = PHA.opcode;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[SP_PAGE_OFFSET + cpu.getSp() + 1], targetValue);
	verifyUnmodifiedStatusFlagsFromPHA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
