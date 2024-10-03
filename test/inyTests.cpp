#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromINY(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
}

// ******************** Zero Page ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, inyWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte yValue = 0x41;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = INY.cycles;

	// Load to A
	memory[PC_RESET] = INY.opcode;
	cpu.setY(yValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getY(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINY(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, inyNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte yValue = 0xD5;
	constexpr byte targetValue = 0xD6;
	constexpr sdword targetCycles = INY.cycles;

	// Load to A, negative value
	memory[PC_RESET] = INY.opcode;
	cpu.setY(yValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getY(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINY(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, inyNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte yValue = 0xFF;
	constexpr byte targetValue = 0x00;
	constexpr sdword targetCycles = INY.cycles;

	// Load to A, null value
	memory[PC_RESET] = INY.opcode;
	cpu.setY(yValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getY(), targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINY(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
