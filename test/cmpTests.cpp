#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromCMP(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
}

// ******************** Immediate ******************** //
// ********** Greater equal test ********** //
TEST_F(CPUTests, cmpImmGEqWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 aValue = 0x42;
	constexpr u8 memValue = 0x12;
	constexpr s32 targetCycles = CMP_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setA(aValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Equal test ********** //
TEST_F(CPUTests, cmpImmEqWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 aValue = 0x42;
	constexpr u8 memValue = 0x42;
	constexpr s32 targetCycles = CMP_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setA(aValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Lesser test ********** //
TEST_F(CPUTests, cmpImmLessWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 aValue = 0x41;
	constexpr u8 memValue = 0x43;
	constexpr s32 targetCycles = CMP_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setA(aValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Greater but sign bit test ********** //
TEST_F(CPUTests, cmpImmGreSignWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 aValue = 0xFF;
	constexpr u8 memValue = 0x12;
	constexpr s32 targetCycles = CMP_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setA(aValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page ******************** //
TEST_F(CPUTests, cmpZPWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 memValue = 0x12;
	constexpr u8 aValue = 0x42;
	constexpr s32 targetCycles = CMP_ZP.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = memValue;
	cpu.setA(aValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page, X ******************** //
TEST_F(CPUTests, cmpZPXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x80;
	constexpr u8 xValue = 0x0F;
	constexpr u8 targetAddress = 0x8F;
	constexpr u8 memValue = 0x12;
	constexpr u8 aValue = 0x42;
	constexpr s32 targetCycles = CMP_ZPX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap test ********** //
TEST_F(CPUTests, cmpZPXWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x80;
	constexpr u8 xValue = 0xFF;
	constexpr u8 targetAddress = 0x7F;
	constexpr u8 memValue = 0x12;
	constexpr u8 aValue = 0x42;
	constexpr s32 targetCycles = CMP_ZPX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, cmpAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u16 address = 0x3224;
	constexpr u8 memValue = 0x12;
	constexpr u8 aValue = 0x42;
	constexpr s32 targetCycles = CMP_ABS.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_ABS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute, X ******************** //
TEST_F(CPUTests, cmpAbsXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 xValue = 0x17;
	constexpr u16 address = 0x323B;
	constexpr u8 memValue = 0x12;
	constexpr u8 aValue = 0x42;
	constexpr s32 targetCycles = CMP_ABSX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, cmpAbsXCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 xValue = 0xFF;
	constexpr u16 address = 0x3323;
	constexpr u8 memValue = 0x12;
	constexpr u8 aValue = 0x42;
	constexpr s32 targetCycles = CMP_ABSX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, cmpAbsXCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0xFF;
	constexpr u8 addressMsb = 0xFF;
	constexpr u8 xValue = 0x01;
	constexpr u16 address = 0x0000;
	constexpr u8 memValue = 0x12;
	constexpr u8 aValue = 0x42;
	constexpr s32 targetCycles = CMP_ABSX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ******************** Absolute, Y ******************** //
TEST_F(CPUTests, cmpAbsYWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 yValue = 0x17;
	constexpr u16 address = 0x323B;
	constexpr u8 memValue = 0x12;
	constexpr u8 aValue = 0x42;
	constexpr s32 targetCycles = CMP_ABSY.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setY(yValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, cmpAbsYCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 yValue = 0xFF;
	constexpr u16 address = 0x3323;
	constexpr u8 memValue = 0x12;
	constexpr u8 aValue = 0x42;
	constexpr s32 targetCycles = CMP_ABSY.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setY(yValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, cmpAbsYCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0xFF;
	constexpr u8 addressMsb = 0xFF;
	constexpr u8 yValue = 0x01;
	constexpr u16 address = 0x0000;
	constexpr u8 memValue = 0x12;
	constexpr u8 aValue = 0x42;
	constexpr s32 targetCycles = CMP_ABSY.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setY(yValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ******************** (Indirect, X) ******************** //
TEST_F(CPUTests, cmpIndXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 xValue = 0x1C;
	constexpr u16 indirectAddress = 0x0040;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x63;
	constexpr u16 targetAddress = 0x634F;
	constexpr u8 memValue = 0x12;
	constexpr u8 aValue = 0x42;
	constexpr s32 targetCycles = CMP_INDX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Full wrap test ********** //
TEST_F(CPUTests, cmpIndXFullyWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 xValue = 0xFF;
	constexpr u16 indirectAddress = 0x0023;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x63;
	constexpr u16 targetAddress = 0x634F;
	constexpr u8 memValue = 0x12;
	constexpr u8 aValue = 0x42;
	constexpr s32 targetCycles = CMP_INDX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Half wrap test ********** //
TEST_F(CPUTests, cmpIndXHalfWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0xFE;
	constexpr u8 xValue = 0x01;
	constexpr u16 indirectAddress = 0x00FF;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x63;
	constexpr u16 targetAddress = 0x634F;
	constexpr u8 memValue = 0x12;
	constexpr u8 aValue = 0x42;
	constexpr s32 targetCycles = CMP_INDX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[0] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** (Indirect), Y ******************** //
TEST_F(CPUTests, cmpIndYWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x63;
	constexpr u8 yValue = 0x1C;
	constexpr u16 targetAddress = 0x636B;
	constexpr u8 memValue = 0x12;
	constexpr u8 aValue = 0x42;
	constexpr s32 targetCycles = CMP_INDY.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_INDY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setY(yValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap & Page cross test ********** //
TEST_F(CPUTests, cmpIndYFullyWrapsAndPageCrosses)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 targetAddressLsb = 0xFF;
	constexpr u8 targetAddressMsb = 0xFF;
	constexpr u8 yValue = 0xFF;
	constexpr u16 targetAddress = 0x00FE;
	constexpr u8 memValue = 0x12;
	constexpr u8 aValue = 0x42;
	constexpr s32 targetCycles = CMP_INDY.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = CMP_INDY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setY(yValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}
