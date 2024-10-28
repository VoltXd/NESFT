#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromINC(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
}

// ******************** Zero Page ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, incZPWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 memValue = 0x41;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = INC_ZP.cycles;

	// Load to A
	memory[TEST_MAIN_ADDRESS] = INC_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[zpAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, incZPNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 memValue = 0xD5;
	constexpr u8 targetValue = 0xD6;
	constexpr s32 targetCycles = INC_ZP.cycles;

	// Load to A, negative value
	memory[TEST_MAIN_ADDRESS] = INC_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[zpAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, incZPNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 memValue = 0xFF;
	constexpr u8 targetValue = 0x00;
	constexpr s32 targetCycles = INC_ZP.cycles;

	// Load to A, null value
	memory[TEST_MAIN_ADDRESS] = INC_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[zpAddress], targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page, X ******************** //
TEST_F(CPUTests, incZPXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x80;
	constexpr u8 xValue = 0x0F;
	constexpr u8 targetAddress = 0x8F;
	constexpr u8 memValue = 0x41;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = INC_ZPX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = INC_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap test ********** //
TEST_F(CPUTests, incZPXWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x80;
	constexpr u8 xValue = 0xFF;
	constexpr u8 targetAddress = 0x7F;
	constexpr u8 memValue = 0x41;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = INC_ZPX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = INC_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, incAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u16 address = 0x3224;
	constexpr u8 memValue = 0x41;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = INC_ABS.cycles;

	// Load to A
	memory[TEST_MAIN_ADDRESS] = INC_ABS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute, X ******************** //
TEST_F(CPUTests, incAbsXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 xValue = 0x17;
	constexpr u16 address = 0x323B;
	constexpr u8 memValue = 0x41;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = INC_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = INC_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, incAbsXCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 xValue = 0xFF;
	constexpr u16 address = 0x3323;
	constexpr u8 memValue = 0x41;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = INC_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = INC_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, incAbsXCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0xFF;
	constexpr u8 addressMsb = 0xFF;
	constexpr u8 xValue = 0x01;
	constexpr u16 address = 0x0000;
	constexpr u8 memValue = 0x41;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = INC_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = INC_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
