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
	constexpr u8 xValue = 0x42;
	constexpr u8 memValue = 0x12;
	constexpr s32 targetCycles = CPX_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CPX_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 xValue = 0x42;
	constexpr u8 memValue = 0x42;
	constexpr s32 targetCycles = CPX_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CPX_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 xValue = 0x41;
	constexpr u8 memValue = 0x43;
	constexpr s32 targetCycles = CPX_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CPX_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 zpAddress = 0x24;
	constexpr u8 memValue = 0x12;
	constexpr u8 xValue = 0x42;
	constexpr s32 targetCycles = CPX_ZP.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CPX_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = memValue;
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u16 address = 0x3224;
	constexpr u8 memValue = 0x12;
	constexpr u8 xValue = 0x42;
	constexpr s32 targetCycles = CPX_ABS.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CPX_ABS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCPX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
