#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromCPX(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
}

// ******************** Immediate ******************** //
// ********** Greater equal test ********** //
TEST_F(CPUTests, cpxImmGEqWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte xValue = 0x42;
	constexpr byte memValue = 0x12;
	constexpr sdword targetCycles = CPX_IMM.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CPX_IMM.opcode;
	memory[PC_RESET + 1] = memValue;
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCPX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Equal test ********** //
TEST_F(CPUTests, cpxImmEqWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte xValue = 0x42;
	constexpr byte memValue = 0x42;
	constexpr sdword targetCycles = CPX_IMM.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CPX_IMM.opcode;
	memory[PC_RESET + 1] = memValue;
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCPX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Lesser test ********** //
TEST_F(CPUTests, cpxImmLessWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte xValue = 0x41;
	constexpr byte memValue = 0x43;
	constexpr sdword targetCycles = CPX_IMM.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CPX_IMM.opcode;
	memory[PC_RESET + 1] = memValue;
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCPX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page ******************** //
TEST_F(CPUTests, cpxZPWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte memValue = 0x12;
	constexpr byte xValue = 0x42;
	constexpr sdword targetCycles = CPX_ZP.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CPX_ZP.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = memValue;
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCPX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, cpxAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr word address = 0x3224;
	constexpr byte memValue = 0x12;
	constexpr byte xValue = 0x42;
	constexpr sdword targetCycles = CPX_ABS.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CPX_ABS.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCPX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
