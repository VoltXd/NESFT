#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromBIT(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
}

// ******************** Zero Page ******************** //
// ******* N V ******* //
TEST_F(CPUTests, bitZPNegOveWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte mask        = 0xFF;
	constexpr byte targetValue = 0b1100'1011;
	constexpr sdword targetCycles = BIT_ZP.cycles;

	// Test bits
	cpu.setA(mask);
	memory[PC_RESET] = BIT_ZP.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getV());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromBIT(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******* Null ******* //
TEST_F(CPUTests, bitZPNullWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte mask        = 0xFF;
	constexpr byte targetValue = 0b0000'0000;
	constexpr sdword targetCycles = BIT_ZP.cycles;

	// Test bits
	cpu.setA(mask);
	memory[PC_RESET] = BIT_ZP.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromBIT(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, bitAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr word address = 0x3224;
	constexpr byte mask        = 0xFF;
	constexpr byte targetValue = 0b1100'1011;
	constexpr sdword targetCycles = BIT_ABS.cycles;

	// Test bits
	cpu.setA(mask);
	memory[PC_RESET] = BIT_ABS.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getV());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromBIT(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
