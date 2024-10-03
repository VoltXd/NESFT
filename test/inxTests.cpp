#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromINX(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
}

// ******************** Zero Page ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, inxWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte xValue = 0x41;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = INX.cycles;

	// Load to A
	memory[PC_RESET] = INX.opcode;
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getX(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, inxNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte xValue = 0xD5;
	constexpr byte targetValue = 0xD6;
	constexpr sdword targetCycles = INX.cycles;

	// Load to A, negative value
	memory[PC_RESET] = INX.opcode;
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getX(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, inxNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte xValue = 0xFF;
	constexpr byte targetValue = 0x00;
	constexpr sdword targetCycles = INX.cycles;

	// Load to A, null value
	memory[PC_RESET] = INX.opcode;
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getX(), targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
