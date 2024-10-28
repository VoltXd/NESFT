#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromDEC(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
}

// ******************** Zero Page ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, decZPWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 memValue = 0x43;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = DEC_ZP.cycles;

	// Load to A
	memory[TEST_MAIN_ADDRESS] = DEC_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[zpAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromDEC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, decZPNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 memValue = 0xD7;
	constexpr u8 targetValue = 0xD6;
	constexpr s32 targetCycles = DEC_ZP.cycles;

	// Load to A, negative value
	memory[TEST_MAIN_ADDRESS] = DEC_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[zpAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromDEC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, decZPNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 memValue = 0x01;
	constexpr u8 targetValue = 0x00;
	constexpr s32 targetCycles = DEC_ZP.cycles;

	// Load to A, null value
	memory[TEST_MAIN_ADDRESS] = DEC_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[zpAddress], targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromDEC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page, X ******************** //
TEST_F(CPUTests, decZPXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x80;
	constexpr u8 xValue = 0x0F;
	constexpr u8 targetAddress = 0x8F;
	constexpr u8 memValue = 0x43;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = DEC_ZPX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = DEC_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromDEC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap test ********** //
TEST_F(CPUTests, decZPXWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x80;
	constexpr u8 xValue = 0xFF;
	constexpr u8 targetAddress = 0x7F;
	constexpr u8 memValue = 0x43;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = DEC_ZPX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = DEC_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromDEC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, decAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u16 address = 0x3224;
	constexpr u8 memValue = 0x43;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = DEC_ABS.cycles;

	// Load to A
	memory[TEST_MAIN_ADDRESS] = DEC_ABS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromDEC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute, X ******************** //
TEST_F(CPUTests, decAbsXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 xValue = 0x17;
	constexpr u16 address = 0x323B;
	constexpr u8 memValue = 0x43;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = DEC_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = DEC_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromDEC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, decAbsXCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 xValue = 0xFF;
	constexpr u16 address = 0x3323;
	constexpr u8 memValue = 0x43;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = DEC_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = DEC_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromDEC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, decAbsXCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0xFF;
	constexpr u8 addressMsb = 0xFF;
	constexpr u8 xValue = 0x01;
	constexpr u16 address = 0x0000;
	constexpr u8 memValue = 0x43;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = DEC_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = DEC_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromDEC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
