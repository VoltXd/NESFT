#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromLDX(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
}

// ******************** Immediate ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, ldxImmPosWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = LDX_IMM.cycles;

	// Load to A
	memory[TEST_MAIN_ADDRESS] = LDX_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = targetValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getX(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, ldxImmNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 targetValue = 0xD8;
	constexpr s32 targetCycles = LDX_IMM.cycles;

	// Load to A, negative value
	memory[TEST_MAIN_ADDRESS] = LDX_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = targetValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getX(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, ldxImmNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 targetValue = 0x00;
	constexpr s32 targetCycles = LDX_IMM.cycles;

	// Load to A, null value
	memory[TEST_MAIN_ADDRESS] = LDX_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = targetValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getX(), targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page ******************** //
TEST_F(CPUTests, ldxZPWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = LDX_ZP.cycles;

	// Load to A
	memory[TEST_MAIN_ADDRESS] = LDX_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = targetValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getX(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page, Y ******************** //
TEST_F(CPUTests, ldxZPYWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x80;
	constexpr u8 yValue = 0x0F;
	constexpr u8 targetAddress = 0x8F;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = LDX_ZPY.cycles;

	// Load to A
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = LDX_ZPY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = targetValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getX(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap test ********** //
TEST_F(CPUTests, ldxZPYWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x80;
	constexpr u8 yValue = 0xFF;
	constexpr u8 targetAddress = 0x7F;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = LDX_ZPY.cycles;

	// Load to A
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = LDX_ZPY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = targetValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getX(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, ldxAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u16 address = 0x3224;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = LDX_ABS.cycles;

	// Load to A
	memory[TEST_MAIN_ADDRESS] = LDX_ABS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = targetValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getX(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute, Y ******************** //
TEST_F(CPUTests, ldxAbsYWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 yValue = 0x17;
	constexpr u16 address = 0x323B;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = LDX_ABSY.cycles;

	// Load to A
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = LDX_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = targetValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getX(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, ldxAbsYCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 yValue = 0xFF;
	constexpr u16 address = 0x3323;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = LDX_ABSY.cycles;

	// Load to A
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = LDX_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = targetValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getX(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, ldxAbsYCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0xFF;
	constexpr u8 addressMsb = 0xFF;
	constexpr u8 yValue = 0x01;
	constexpr u16 address = 0x0000;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = LDX_ABSY.cycles;

	// Load to A
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = LDX_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = targetValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getX(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDX(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}
