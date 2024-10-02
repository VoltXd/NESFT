#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromAND(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
}

// ******************** Immediate ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, andImmPosWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte aValue      = 0b1100'1010;
	constexpr byte memValue    = 0b0111'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = AND_IMM.cycles;

	// A & mem
	cpu.setA(aValue);
	memory[PC_RESET] = AND_IMM.opcode;
	memory[PC_RESET + 1] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, andImmNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte aValue      = 0b1101'1001;
	constexpr byte memValue    = 0b1111'1010;
	constexpr byte targetValue = 0b1101'1000;
	constexpr sdword targetCycles = AND_IMM.cycles;

	// A & mem, negative value
	cpu.setA(aValue);
	memory[PC_RESET] = AND_IMM.opcode;
	memory[PC_RESET + 1] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, andImmNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte aValue      = 0b0101'0010;
	constexpr byte memValue    = 0b1010'1000;
	constexpr byte targetValue = 0b0000'0000;
	constexpr sdword targetCycles = AND_IMM.cycles;

	// A & mem, null value
	cpu.setA(aValue);
	memory[PC_RESET] = AND_IMM.opcode;
	memory[PC_RESET + 1] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page ******************** //
TEST_F(CPUTests, andZPWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte aValue      = 0b1100'1010;
	constexpr byte memValue    = 0b0111'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = AND_ZP.cycles;

	// A & mem
	cpu.setA(aValue);
	memory[PC_RESET] = AND_ZP.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page, X ******************** //
TEST_F(CPUTests, andZPXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0x0F;
	constexpr byte targetAddress = 0x8F;
	constexpr byte aValue      = 0b1100'1010;
	constexpr byte memValue    = 0b0111'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = AND_ZPX.cycles;

	// A & mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[PC_RESET] = AND_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[targetAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap test ********** //
TEST_F(CPUTests, andZPXWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0xFF;
	constexpr byte targetAddress = 0x7F;
	constexpr byte aValue      = 0b1100'1010;
	constexpr byte memValue    = 0b0111'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = AND_ZPX.cycles;

	// A & mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[PC_RESET] = AND_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[targetAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, andAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr word address = 0x3224;
	constexpr byte aValue      = 0b1100'1010;
	constexpr byte memValue    = 0b0111'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = AND_ABS.cycles;

	// A & mem
	cpu.setA(aValue);
	memory[PC_RESET] = AND_ABS.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute, X ******************** //
TEST_F(CPUTests, andAbsXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte aValue      = 0b1100'1010;
	constexpr byte memValue    = 0b0111'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = AND_ABSX.cycles;

	// A & mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[PC_RESET] = AND_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, andAbsXCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0xFF;
	constexpr word address = 0x3323;
	constexpr byte aValue      = 0b1100'1010;
	constexpr byte memValue    = 0b0111'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = AND_ABSX.cycles;

	// A & mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[PC_RESET] = AND_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, andAbsXCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0xFF;
	constexpr byte addressMsb = 0xFF;
	constexpr byte xValue = 0x01;
	constexpr word address = 0x0000;
	constexpr byte aValue      = 0b1100'1010;
	constexpr byte memValue    = 0b0111'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = AND_ABSX.cycles;

	// A & mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[PC_RESET] = AND_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ******************** Absolute, Y ******************** //
TEST_F(CPUTests, andAbsYWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte yValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte aValue      = 0b1100'1010;
	constexpr byte memValue    = 0b0111'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = AND_ABSY.cycles;

	// A & mem
	cpu.setA(aValue);
	cpu.setY(yValue);
	memory[PC_RESET] = AND_ABSY.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, andAbsYCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte yValue = 0xFF;
	constexpr word address = 0x3323;
	constexpr byte aValue      = 0b1100'1010;
	constexpr byte memValue    = 0b0111'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = AND_ABSY.cycles;

	// A & mem
	cpu.setA(aValue);
	cpu.setY(yValue);
	memory[PC_RESET] = AND_ABSY.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, andAbsYCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0xFF;
	constexpr byte addressMsb = 0xFF;
	constexpr byte yValue = 0x01;
	constexpr word address = 0x0000;
	constexpr byte aValue      = 0b1100'1010;
	constexpr byte memValue    = 0b0111'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = AND_ABSY.cycles;

	// A & mem
	cpu.setA(aValue);
	cpu.setY(yValue);
	memory[PC_RESET] = AND_ABSY.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ******************** (Indirect, X) ******************** //
TEST_F(CPUTests, andIndXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte xValue = 0x1C;
	constexpr word indirectAddress = 0x0040;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr word targetAddress = 0x634F;
	constexpr byte aValue      = 0b1100'1010;
	constexpr byte memValue    = 0b0111'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = AND_INDX.cycles;

	// A & mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[PC_RESET] = AND_INDX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Full wrap test ********** //
TEST_F(CPUTests, andIndXFullyWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte xValue = 0xFF;
	constexpr word indirectAddress = 0x0023;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr word targetAddress = 0x634F;
	constexpr byte aValue      = 0b1100'1010;
	constexpr byte memValue    = 0b0111'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = AND_INDX.cycles;

	// A & mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[PC_RESET] = AND_INDX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Half wrap test ********** //
TEST_F(CPUTests, andIndXHalfWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0xFE;
	constexpr byte xValue = 0x01;
	constexpr word indirectAddress = 0x00FF;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr word targetAddress = 0x634F;
	constexpr byte aValue      = 0b1100'1010;
	constexpr byte memValue    = 0b0111'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = AND_INDX.cycles;

	// A & mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[PC_RESET] = AND_INDX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[0] = targetAddressMsb;
	memory[targetAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** (Indirect), Y ******************** //
TEST_F(CPUTests, andIndYWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr byte yValue = 0x1C;
	constexpr word targetAddress = 0x636B;
	constexpr byte aValue      = 0b1100'1010;
	constexpr byte memValue    = 0b0111'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = AND_INDY.cycles;

	// A & mem
	cpu.setA(aValue);
	cpu.setY(yValue);
	memory[PC_RESET] = AND_INDY.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap & Page cross test ********** //
TEST_F(CPUTests, andIndYFullyWrapsAndPageCrosses)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte targetAddressLsb = 0xFF;
	constexpr byte targetAddressMsb = 0xFF;
	constexpr byte yValue = 0xFF;
	constexpr word targetAddress = 0x00FE;
	constexpr byte aValue      = 0b1100'1010;
	constexpr byte memValue    = 0b0111'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = AND_INDY.cycles;

	// A & mem
	cpu.setA(aValue);
	cpu.setY(yValue);
	memory[PC_RESET] = AND_INDY.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromAND(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}
