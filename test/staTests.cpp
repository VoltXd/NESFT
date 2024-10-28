#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromSTA(const CPU& cpu, const CPU& cpuInitialState)
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
TEST_F(CPUTests, staZPWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STA_ZP.cycles;

	// Store A
	memory[TEST_MAIN_ADDRESS] = STA_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	cpu.setA(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[zpAddress], targetValue);
	verifyUnmodifiedStatusFlagsFromSTA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page, X ******************** //
// ********** No wrap test ********** //
TEST_F(CPUTests, staZPXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x80;
	constexpr u8 xValue = 0x0F;
	constexpr u8 targetAddress = 0x8F;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STA_ZPX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = STA_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	cpu.setA(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap test ********** //
TEST_F(CPUTests, staZPXWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x80;
	constexpr u8 xValue = 0xFF;
	constexpr u8 targetAddress = 0x7F;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STA_ZPX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = STA_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	cpu.setA(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, staAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u16 address = 0x3224;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STA_ABS.cycles;

	// Store A
	memory[TEST_MAIN_ADDRESS] = STA_ABS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	cpu.setA(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute, X ******************** //
TEST_F(CPUTests, staAbsXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 xValue = 0x17;
	constexpr u16 address = 0x323B;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STA_ABSX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = STA_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	cpu.setA(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, staAbsXCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 xValue = 0xFF;
	constexpr u16 address = 0x3323;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STA_ABSX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = STA_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	cpu.setA(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, staAbsXCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0xFF;
	constexpr u8 addressMsb = 0xFF;
	constexpr u8 xValue = 0x01;
	constexpr u16 address = 0x0000;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STA_ABSX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = STA_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	cpu.setA(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute, Y ******************** //
TEST_F(CPUTests, staAbsYPosWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 yValue = 0x17;
	constexpr u16 address = 0x323B;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STA_ABSY.cycles;

	// Store A
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = STA_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	cpu.setA(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, staAbsYCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 yValue = 0xFF;
	constexpr u16 address = 0x3323;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STA_ABSY.cycles;

	// Store A
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = STA_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	cpu.setA(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, staAbsYCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0xFF;
	constexpr u8 addressMsb = 0xFF;
	constexpr u8 yValue = 0x01;
	constexpr u16 address = 0x0000;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STA_ABSY.cycles;

	// Store A
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = STA_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	cpu.setA(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** (Indirect, X) ******************** //
TEST_F(CPUTests, staIndXPosWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 xValue = 0x1C;
	constexpr u16 indirectAddress = 0x0040;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x63;
	constexpr u16 targetAddress = 0x634F;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STA_INDX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = STA_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	cpu.setA(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Full wrap test ********** //
TEST_F(CPUTests, staIndXFullyWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 xValue = 0xFF;
	constexpr u16 indirectAddress = 0x0023;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x63;
	constexpr u16 targetAddress = 0x634F;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STA_INDX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = STA_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	cpu.setA(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Half wrap test ********** //
TEST_F(CPUTests, staIndXHalfWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0xFE;
	constexpr u8 xValue = 0x01;
	constexpr u16 indirectAddress = 0x00FF;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x63;
	constexpr u16 targetAddress = 0x634F;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STA_INDX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = STA_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[0] = targetAddressMsb;
	cpu.setA(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** (Indirect), Y ******************** //
TEST_F(CPUTests, staIndYPosWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x63;
	constexpr u8 yValue = 0x1C;
	constexpr u16 targetAddress = 0x636B;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STA_INDY.cycles;

	// Store A
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = STA_INDY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	cpu.setA(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap & Page cross test ********** //
TEST_F(CPUTests, staIndYFullyWrapsAndPageCrosses)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 targetAddressLsb = 0xFF;
	constexpr u8 targetAddressMsb = 0xFF;
	constexpr u8 yValue = 0xFF;
	constexpr u16 targetAddress = 0x00FE;
	constexpr u8 targetValue = 0x42;
	constexpr s32 targetCycles = STA_INDY.cycles;

	// Store A
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = STA_INDY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	cpu.setA(targetValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
