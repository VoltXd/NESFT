#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromORA(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
}

// ******************** Immediate ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, oraImmPosWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 aValue      = 0b0100'0000;
	constexpr u8 memValue    = 0b0000'0010;
	constexpr u8 targetValue = 0b0100'0010;
	constexpr s32 targetCycles = ORA_IMM.cycles;

	// A | mem
	cpu.setA(aValue);
	memory[TEST_MAIN_ADDRESS] = ORA_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, oraImmNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 aValue      = 0b1000'1000;
	constexpr u8 memValue    = 0b0101'0000;
	constexpr u8 targetValue = 0b1101'1000;
	constexpr s32 targetCycles = ORA_IMM.cycles;

	// A | mem, negative value
	cpu.setA(aValue);
	memory[TEST_MAIN_ADDRESS] = ORA_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, oraImmNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 aValue      = 0b0000'0000;
	constexpr u8 memValue    = 0b0000'0000;
	constexpr u8 targetValue = 0b0000'0000;
	constexpr s32 targetCycles = ORA_IMM.cycles;

	// A | mem, null value
	cpu.setA(aValue);
	memory[TEST_MAIN_ADDRESS] = ORA_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page ******************** //
TEST_F(CPUTests, oraZPWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 aValue      = 0b0100'0000;
	constexpr u8 memValue    = 0b0000'0010;
	constexpr u8 targetValue = 0b0100'0010;
	constexpr s32 targetCycles = ORA_ZP.cycles;

	// A | mem
	cpu.setA(aValue);
	memory[TEST_MAIN_ADDRESS] = ORA_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page, X ******************** //
TEST_F(CPUTests, oraZPXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x80;
	constexpr u8 xValue = 0x0F;
	constexpr u8 targetAddress = 0x8F;
	constexpr u8 aValue      = 0b0100'0000;
	constexpr u8 memValue    = 0b0000'0010;
	constexpr u8 targetValue = 0b0100'0010;
	constexpr s32 targetCycles = ORA_ZPX.cycles;

	// A | mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = ORA_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap test ********** //
TEST_F(CPUTests, oraZPXWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x80;
	constexpr u8 xValue = 0xFF;
	constexpr u8 targetAddress = 0x7F;
	constexpr u8 aValue      = 0b0100'0000;
	constexpr u8 memValue    = 0b0000'0010;
	constexpr u8 targetValue = 0b0100'0010;
	constexpr s32 targetCycles = ORA_ZPX.cycles;

	// A | mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = ORA_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, oraAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u16 address = 0x3224;
	constexpr u8 aValue      = 0b0100'0000;
	constexpr u8 memValue    = 0b0000'0010;
	constexpr u8 targetValue = 0b0100'0010;
	constexpr s32 targetCycles = ORA_ABS.cycles;

	// A | mem
	cpu.setA(aValue);
	memory[TEST_MAIN_ADDRESS] = ORA_ABS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute, X ******************** //
TEST_F(CPUTests, oraAbsXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 xValue = 0x17;
	constexpr u16 address = 0x323B;
	constexpr u8 aValue      = 0b0100'0000;
	constexpr u8 memValue    = 0b0000'0010;
	constexpr u8 targetValue = 0b0100'0010;
	constexpr s32 targetCycles = ORA_ABSX.cycles;

	// A | mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = ORA_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, oraAbsXCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 xValue = 0xFF;
	constexpr u16 address = 0x3323;
	constexpr u8 aValue      = 0b0100'0000;
	constexpr u8 memValue    = 0b0000'0010;
	constexpr u8 targetValue = 0b0100'0010;
	constexpr s32 targetCycles = ORA_ABSX.cycles;

	// A | mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = ORA_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, oraAbsXCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0xFF;
	constexpr u8 addressMsb = 0xFF;
	constexpr u8 xValue = 0x01;
	constexpr u16 address = 0x0000;
	constexpr u8 aValue      = 0b0100'0000;
	constexpr u8 memValue    = 0b0000'0010;
	constexpr u8 targetValue = 0b0100'0010;
	constexpr s32 targetCycles = ORA_ABSX.cycles;

	// A | mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = ORA_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ******************** Absolute, Y ******************** //
TEST_F(CPUTests, oraAbsYWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 yValue = 0x17;
	constexpr u16 address = 0x323B;
	constexpr u8 aValue      = 0b0100'0000;
	constexpr u8 memValue    = 0b0000'0010;
	constexpr u8 targetValue = 0b0100'0010;
	constexpr s32 targetCycles = ORA_ABSY.cycles;

	// A | mem
	cpu.setA(aValue);
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = ORA_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, oraAbsYCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 yValue = 0xFF;
	constexpr u16 address = 0x3323;
	constexpr u8 aValue      = 0b0100'0000;
	constexpr u8 memValue    = 0b0000'0010;
	constexpr u8 targetValue = 0b0100'0010;
	constexpr s32 targetCycles = ORA_ABSY.cycles;

	// A | mem
	cpu.setA(aValue);
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = ORA_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, oraAbsYCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 addressLsb = 0xFF;
	constexpr u8 addressMsb = 0xFF;
	constexpr u8 yValue = 0x01;
	constexpr u16 address = 0x0000;
	constexpr u8 aValue      = 0b0100'0000;
	constexpr u8 memValue    = 0b0000'0010;
	constexpr u8 targetValue = 0b0100'0010;
	constexpr s32 targetCycles = ORA_ABSY.cycles;

	// A | mem
	cpu.setA(aValue);
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = ORA_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ******************** (Indirect, X) ******************** //
TEST_F(CPUTests, oraIndXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 xValue = 0x1C;
	constexpr u16 indirectAddress = 0x0040;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x63;
	constexpr u16 targetAddress = 0x634F;
	constexpr u8 aValue      = 0b0100'0000;
	constexpr u8 memValue    = 0b0000'0010;
	constexpr u8 targetValue = 0b0100'0010;
	constexpr s32 targetCycles = ORA_INDX.cycles;

	// A | mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = ORA_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Full wrap test ********** //
TEST_F(CPUTests, oraIndXFullyWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 xValue = 0xFF;
	constexpr u16 indirectAddress = 0x0023;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x63;
	constexpr u16 targetAddress = 0x634F;
	constexpr u8 aValue      = 0b0100'0000;
	constexpr u8 memValue    = 0b0000'0010;
	constexpr u8 targetValue = 0b0100'0010;
	constexpr s32 targetCycles = ORA_INDX.cycles;

	// A | mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = ORA_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Half wrap test ********** //
TEST_F(CPUTests, oraIndXHalfWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0xFE;
	constexpr u8 xValue = 0x01;
	constexpr u16 indirectAddress = 0x00FF;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x63;
	constexpr u16 targetAddress = 0x634F;
	constexpr u8 aValue      = 0b0100'0000;
	constexpr u8 memValue    = 0b0000'0010;
	constexpr u8 targetValue = 0b0100'0010;
	constexpr s32 targetCycles = ORA_INDX.cycles;

	// A | mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = ORA_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[0] = targetAddressMsb;
	memory[targetAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** (Indirect), Y ******************** //
TEST_F(CPUTests, oraIndYWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x63;
	constexpr u8 yValue = 0x1C;
	constexpr u16 targetAddress = 0x636B;
	constexpr u8 aValue      = 0b0100'0000;
	constexpr u8 memValue    = 0b0000'0010;
	constexpr u8 targetValue = 0b0100'0010;
	constexpr s32 targetCycles = ORA_INDY.cycles;

	// A | mem
	cpu.setA(aValue);
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = ORA_INDY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap & Page cross test ********** //
TEST_F(CPUTests, oraIndYFullyWrapsAndPageCrosses)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 zpAddress = 0x24;
	constexpr u8 targetAddressLsb = 0xFF;
	constexpr u8 targetAddressMsb = 0xFF;
	constexpr u8 yValue = 0xFF;
	constexpr u16 targetAddress = 0x00FE;
	constexpr u8 aValue      = 0b0100'0000;
	constexpr u8 memValue    = 0b0000'0010;
	constexpr u8 targetValue = 0b0100'0010;
	constexpr s32 targetCycles = ORA_INDY.cycles;

	// A | mem
	cpu.setA(aValue);
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = ORA_INDY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromORA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}
