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
	constexpr u8 zpAddress = 0x24;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STX_ZP.cycles;

	// Store A
	memory[TEST_MAIN_ADDRESS] = STX_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	cpu.setX(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 zpAddress = 0x80;
	constexpr u8 yValue = 0x0F;
	constexpr u8 targetAddress = 0x8F;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STX_ZPY.cycles;

	// Store A
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = STX_ZPY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	cpu.setX(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 zpAddress = 0x80;
	constexpr u8 yValue = 0xFF;
	constexpr u8 targetAddress = 0x7F;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STX_ZPY.cycles;

	// Store A
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = STX_ZPY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	cpu.setX(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u16 address = 0x3224;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STX_ABS.cycles;

	// Store A
	memory[TEST_MAIN_ADDRESS] = STX_ABS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	cpu.setX(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
