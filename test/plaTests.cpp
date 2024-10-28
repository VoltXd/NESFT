#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromPLA(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
}

// ********** Positive test ********** //
TEST_F(CPUTests, plaPosWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = PLA.cycles;

	// Pull A
	cpu.setSp(SP_RESET - 1);
	memory[TEST_MAIN_ADDRESS] = PLA.opcode;
	memory[SP_PAGE_OFFSET | SP_RESET] = targetValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromPLA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, plaNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 targetValue = 0x82;
	constexpr s32 targetCycles = PLA.cycles;

	// Pull A
	cpu.setSp(SP_RESET - 1);
	memory[TEST_MAIN_ADDRESS] = PLA.opcode;
	memory[SP_PAGE_OFFSET | SP_RESET] = targetValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromPLA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, plaNullWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 targetValue = 0x00;
	constexpr s32 targetCycles = PLA.cycles;

	// Pull A
	cpu.setSp(SP_RESET - 1);
	memory[TEST_MAIN_ADDRESS] = PLA.opcode;
	memory[SP_PAGE_OFFSET | SP_RESET] = targetValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromPLA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
