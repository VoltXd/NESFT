#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromPLP(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getZ(), cpuInitialState.getZ());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
	EXPECT_EQ(cpu.getN(), cpuInitialState.getN());
}

TEST_F(CPUTests, plpWorks)
{
	// Target values
	cpu.setC(1);
	cpu.setN(1);
	const CPU cpuInitialState = cpu;
	constexpr sdword targetCycles = PLP.cycles;

	// Pull processor status
	// NVUB'DIZC = 0b1011'0001
	cpu.setSp(SP_RESET - 1);
	memory[PC_RESET] = PLP.opcode;
	memory[SP_PAGE_OFFSET | SP_RESET] = 0b1011'0001;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	byte processorStatus = (cpu.getC() & 0x01) << 0 |
	                       (cpu.getZ() & 0x01) << 1 |
	                       (cpu.getI() & 0x01) << 2 |
	                       (cpu.getD() & 0x01) << 3 |
	                       (cpu.getB() & 0x01) << 4 |
	                       (1          & 0x01) << 5 |
	                       (cpu.getV() & 0x01) << 6 |
	                       (cpu.getN() & 0x01) << 7;
	EXPECT_EQ(processorStatus, 0b1011'0001);
	verifyUnmodifiedStatusFlagsFromPLP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
