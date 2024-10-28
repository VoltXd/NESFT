#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromCPY(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
}

// ******************** Immediate ******************** //
// ********** Greater equal test ********** //
TEST_F(CPUTests, cpyImmGEqWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 yValue = 0x42;
	constexpr u8 memValue = 0x12;
	constexpr s32 targetCycles = CPY_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CPY_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setY(yValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCPY(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Equal test ********** //
TEST_F(CPUTests, cpyImmEqWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 yValue = 0x42;
	constexpr u8 memValue = 0x42;
	constexpr s32 targetCycles = CPY_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CPY_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setY(yValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCPY(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Lesser test ********** //
TEST_F(CPUTests, cpyImmLessWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 yValue = 0x41;
	constexpr u8 memValue = 0x43;
	constexpr s32 targetCycles = CPY_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CPY_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setY(yValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCPY(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page ******************** //
TEST_F(CPUTests, cpyZPWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 memValue = 0x12;
	constexpr u8 yValue = 0x42;
	constexpr s32 targetCycles = CPY_ZP.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CPY_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = memValue;
	cpu.setY(yValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCPY(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, cpyAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u16 address = 0x3224;
	constexpr u8 memValue = 0x12;
	constexpr u8 yValue = 0x42;
	constexpr s32 targetCycles = CPY_ABS.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CPY_ABS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setY(yValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCPY(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
