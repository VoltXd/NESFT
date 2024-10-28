#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromROL(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
}

// ******************** Accumulator ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, rolAccWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 inputValue = 0b0001'1001;
	constexpr u8 targetValue = 0b0011'0011;
	constexpr s32 targetCycles = ROL_ACC.cycles;

	// Load to A
	memory[TEST_MAIN_ADDRESS] = ROL_ACC.opcode;
	cpu.setA(inputValue);
	cpu.setC(1);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, rolAccNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 inputValue = 0b0101'1001;
	constexpr u8 targetValue = 0b1011'0010;
	constexpr s32 targetCycles = ROL_ACC.cycles;

	// Load to A
	memory[TEST_MAIN_ADDRESS] = ROL_ACC.opcode;
	cpu.setA(inputValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null & carry test ********** //
TEST_F(CPUTests, rolAccNullCarryWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 inputValue = 0b1000'0000;
	constexpr u8 targetValue = 0b0000'0000;
	constexpr s32 targetCycles = ROL_ACC.cycles;

	// Load to A
	memory[TEST_MAIN_ADDRESS] = ROL_ACC.opcode;
	cpu.setA(inputValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_TRUE(cpu.getC());
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page ******************** //
TEST_F(CPUTests, rolZPWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 inputValue = 0b0001'1001;
	constexpr u8 targetValue = 0b0011'0011;
	constexpr s32 targetCycles = ROL_ZP.cycles;

	// Load to A
	memory[TEST_MAIN_ADDRESS] = ROL_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = inputValue;
	cpu.setC(1);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[zpAddress], targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page, X ******************** //
TEST_F(CPUTests, rolZPXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x80;
	constexpr u8 xValue = 0x0F;
	constexpr u8 targetAddress = 0x8F;
	constexpr u8 inputValue = 0b0001'1001;
	constexpr u8 targetValue = 0b0011'0011;
	constexpr s32 targetCycles = ROL_ZPX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = ROL_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = inputValue;
	cpu.setC(1);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap test ********** //
TEST_F(CPUTests, rolZPXWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x80;
	constexpr u8 xValue = 0xFF;
	constexpr u8 targetAddress = 0x7F;
	constexpr u8 inputValue = 0b0001'1001;
	constexpr u8 targetValue = 0b0011'0011;
	constexpr s32 targetCycles = ROL_ZPX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = ROL_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = inputValue;
	cpu.setC(1);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, rolAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u16 address = 0x3224;
	constexpr u8 inputValue = 0b0001'1001;
	constexpr u8 targetValue = 0b0011'0011;
	constexpr s32 targetCycles = ROL_ABS.cycles;

	// Load to A
	memory[TEST_MAIN_ADDRESS] = ROL_ABS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = inputValue;
	cpu.setC(1);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute, X ******************** //
TEST_F(CPUTests, rolAbsXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 xValue = 0x17;
	constexpr u16 address = 0x323B;
	constexpr u8 inputValue = 0b0001'1001;
	constexpr u8 targetValue = 0b0011'0011;
	constexpr s32 targetCycles = ROL_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = ROL_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = inputValue;
	cpu.setC(1);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, rolAbsXCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 xValue = 0xFF;
	constexpr u16 address = 0x3323;
	constexpr u8 inputValue = 0b0001'1001;
	constexpr u8 targetValue = 0b0011'0011;
	constexpr s32 targetCycles = ROL_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = ROL_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = inputValue;
	cpu.setC(1);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, rolAbsXCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0xFF;
	constexpr u8 addressMsb = 0xFF;
	constexpr u8 xValue = 0x01;
	constexpr u16 address = 0x0000;
	constexpr u8 inputValue = 0b0001'1001;
	constexpr u8 targetValue = 0b0011'0011;
	constexpr s32 targetCycles = ROL_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = ROL_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = inputValue;
	cpu.setC(1);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
