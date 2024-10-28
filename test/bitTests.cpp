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
	constexpr u8 zpAddress = 0x24;
	constexpr u8 mask        = 0xFF;
	constexpr u8 targetValue = 0b1100'1011;
	constexpr s32 targetCycles = BIT_ZP.cycles;

	// Test bits
	cpu.setA(mask);
	memory[TEST_MAIN_ADDRESS] = BIT_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = targetValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 zpAddress = 0x24;
	constexpr u8 mask        = 0xFF;
	constexpr u8 targetValue = 0b0000'0000;
	constexpr s32 targetCycles = BIT_ZP.cycles;

	// Test bits
	cpu.setA(mask);
	memory[TEST_MAIN_ADDRESS] = BIT_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = targetValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u16 address = 0x3224;
	constexpr u8 mask        = 0xFF;
	constexpr u8 targetValue = 0b1100'1011;
	constexpr s32 targetCycles = BIT_ABS.cycles;

	// Test bits
	cpu.setA(mask);
	memory[TEST_MAIN_ADDRESS] = BIT_ABS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = targetValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getV());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromBIT(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
