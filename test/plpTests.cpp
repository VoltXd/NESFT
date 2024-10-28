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
	constexpr s32 targetCycles = PLP.cycles;

	// Pull processor status
	// NVUB'DIZC = 0b1011'0001
	cpu.setSp(SP_RESET - 4);
	memory[TEST_MAIN_ADDRESS] = PLP.opcode;
	memory[SP_PAGE_OFFSET | 0xFC] = 0b1011'0101;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	u8 processorStatus = (cpu.getC() & 0x01) << 0 |
	                       (cpu.getZ() & 0x01) << 1 |
	                       (cpu.getI() & 0x01) << 2 |
	                       (cpu.getD() & 0x01) << 3 |
	                       (cpu.getB() & 0x01) << 4 |
	                       (1          & 0x01) << 5 |
	                       (cpu.getV() & 0x01) << 6 |
	                       (cpu.getN() & 0x01) << 7;
	EXPECT_EQ(processorStatus, 0b1011'0101);
	verifyUnmodifiedStatusFlagsFromPLP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
