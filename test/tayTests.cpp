#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromTAY(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
}

// ********** Positive test ********** //
TEST_F(CPUTests, tayPosWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = TAY.cycles;

	// Transfert A to X
	memory[TEST_MAIN_ADDRESS] = TAY.opcode;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getY(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromTAY(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, tayNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte targetValue = 0xD8;
	constexpr sdword targetCycles = TAY.cycles;

	// Transfert A to X
	memory[TEST_MAIN_ADDRESS] = TAY.opcode;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getY(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromTAY(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, tayNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte targetValue = 0x00;
	constexpr sdword targetCycles = TAY.cycles;

	// Transfert A to X
	memory[TEST_MAIN_ADDRESS] = TAY.opcode;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getY(), targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromTAY(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
