#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromSTX(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getZ(), cpuInitialState.getZ());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
	EXPECT_EQ(cpu.getN(), cpuInitialState.getN());
}

// ******************** Zero Page ******************** //
TEST_F(CPUTests, stxZPWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STX_ZP.cycles;

	// Store A
	memory[PC_RESET] = STX_ZP.opcode;
	memory[PC_RESET + 1] = zpAddress;
	cpu.setX(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[zpAddress], targetValue);
	verifyUnmodifiedStatusFlagsFromSTX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page, Y ******************** //
// ********** No wrap test ********** //
TEST_F(CPUTests, stxZPXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x80;
	constexpr byte yValue = 0x0F;
	constexpr byte targetAddress = 0x8F;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STX_ZPY.cycles;

	// Store A
	cpu.setY(yValue);
	memory[PC_RESET] = STX_ZPY.opcode;
	memory[PC_RESET + 1] = zpAddress;
	cpu.setX(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap test ********** //
TEST_F(CPUTests, stxZPXWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x80;
	constexpr byte yValue = 0xFF;
	constexpr byte targetAddress = 0x7F;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STX_ZPY.cycles;

	// Store A
	cpu.setY(yValue);
	memory[PC_RESET] = STX_ZPY.opcode;
	memory[PC_RESET + 1] = zpAddress;
	cpu.setX(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, stxAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr word address = 0x3224;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STX_ABS.cycles;

	// Store A
	memory[PC_RESET] = STX_ABS.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	cpu.setX(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
