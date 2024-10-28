#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromSTY(const CPU& cpu, const CPU& cpuInitialState)
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
TEST_F(CPUTests, styZPWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STY_ZP.cycles;

	// Store A
	memory[TEST_MAIN_ADDRESS] = STY_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	cpu.setY(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[zpAddress], targetValue);
	verifyUnmodifiedStatusFlagsFromSTY(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page, X ******************** //
// ********** No wrap test ********** //
TEST_F(CPUTests, styZPXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x80;
	constexpr u8 xValue = 0x0F;
	constexpr u8 targetAddress = 0x8F;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STY_ZPX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = STY_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	cpu.setY(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTY(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap test ********** //
TEST_F(CPUTests, styZPXWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x80;
	constexpr u8 xValue = 0xFF;
	constexpr u8 targetAddress = 0x7F;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STY_ZPX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = STY_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	cpu.setY(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTY(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, styAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u16 address = 0x3224;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STY_ABS.cycles;

	// Store A
	memory[TEST_MAIN_ADDRESS] = STY_ABS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	cpu.setY(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTY(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
