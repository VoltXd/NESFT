#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromEOR(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
}

// ******************** Immediate ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, eorImmPosWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte aValue      = 0b1111'0000;
	constexpr byte memValue    = 0b1011'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = EOR_IMM.cycles;

	// A ^ mem
	cpu.setA(aValue);
	memory[TEST_MAIN_ADDRESS] = EOR_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, eorImmNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte aValue      = 0b1010'1011;
	constexpr byte memValue    = 0b0111'0011;
	constexpr byte targetValue = 0b1101'1000;
	constexpr sdword targetCycles = EOR_IMM.cycles;

	// A ^ mem, negative value
	cpu.setA(aValue);
	memory[TEST_MAIN_ADDRESS] = EOR_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, eorImmNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte aValue      = 0b1001'1000;
	constexpr byte memValue    = 0b1001'1000;
	constexpr byte targetValue = 0b0000'0000;
	constexpr sdword targetCycles = EOR_IMM.cycles;

	// A ^ mem, null value
	cpu.setA(aValue);
	memory[TEST_MAIN_ADDRESS] = EOR_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page ******************** //
TEST_F(CPUTests, eorZPWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte aValue      = 0b1111'0000;
	constexpr byte memValue    = 0b1011'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = EOR_ZP.cycles;

	// A ^ mem
	cpu.setA(aValue);
	memory[TEST_MAIN_ADDRESS] = EOR_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page, X ******************** //
TEST_F(CPUTests, eorZPXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0x0F;
	constexpr byte targetAddress = 0x8F;
	constexpr byte aValue      = 0b1111'0000;
	constexpr byte memValue    = 0b1011'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = EOR_ZPX.cycles;

	// A ^ mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = EOR_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap test ********** //
TEST_F(CPUTests, eorZPXWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0xFF;
	constexpr byte targetAddress = 0x7F;
	constexpr byte aValue      = 0b1111'0000;
	constexpr byte memValue    = 0b1011'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = EOR_ZPX.cycles;

	// A ^ mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = EOR_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, eorAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr word address = 0x3224;
	constexpr byte aValue      = 0b1111'0000;
	constexpr byte memValue    = 0b1011'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = EOR_ABS.cycles;

	// A ^ mem
	cpu.setA(aValue);
	memory[TEST_MAIN_ADDRESS] = EOR_ABS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute, X ******************** //
TEST_F(CPUTests, eorAbsXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte aValue      = 0b1111'0000;
	constexpr byte memValue    = 0b1011'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = EOR_ABSX.cycles;

	// A ^ mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = EOR_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, eorAbsXCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0xFF;
	constexpr word address = 0x3323;
	constexpr byte aValue      = 0b1111'0000;
	constexpr byte memValue    = 0b1011'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = EOR_ABSX.cycles;

	// A ^ mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = EOR_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, eorAbsXCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0xFF;
	constexpr byte addressMsb = 0xFF;
	constexpr byte xValue = 0x01;
	constexpr word address = 0x0000;
	constexpr byte aValue      = 0b1111'0000;
	constexpr byte memValue    = 0b1011'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = EOR_ABSX.cycles;

	// A ^ mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = EOR_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ******************** Absolute, Y ******************** //
TEST_F(CPUTests, eorAbsYWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte yValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte aValue      = 0b1111'0000;
	constexpr byte memValue    = 0b1011'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = EOR_ABSY.cycles;

	// A ^ mem
	cpu.setA(aValue);
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = EOR_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, eorAbsYCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte yValue = 0xFF;
	constexpr word address = 0x3323;
	constexpr byte aValue      = 0b1111'0000;
	constexpr byte memValue    = 0b1011'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = EOR_ABSY.cycles;

	// A ^ mem
	cpu.setA(aValue);
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = EOR_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, eorAbsYCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0xFF;
	constexpr byte addressMsb = 0xFF;
	constexpr byte yValue = 0x01;
	constexpr word address = 0x0000;
	constexpr byte aValue      = 0b1111'0000;
	constexpr byte memValue    = 0b1011'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = EOR_ABSY.cycles;

	// A ^ mem
	cpu.setA(aValue);
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = EOR_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ******************** (Indirect, X) ******************** //
TEST_F(CPUTests, eorIndXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte xValue = 0x1C;
	constexpr word indirectAddress = 0x0040;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr word targetAddress = 0x634F;
	constexpr byte aValue      = 0b1111'0000;
	constexpr byte memValue    = 0b1011'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = EOR_INDX.cycles;

	// A ^ mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = EOR_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Full wrap test ********** //
TEST_F(CPUTests, eorIndXFullyWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte xValue = 0xFF;
	constexpr word indirectAddress = 0x0023;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr word targetAddress = 0x634F;
	constexpr byte aValue      = 0b1111'0000;
	constexpr byte memValue    = 0b1011'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = EOR_INDX.cycles;

	// A ^ mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = EOR_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Half wrap test ********** //
TEST_F(CPUTests, eorIndXHalfWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0xFE;
	constexpr byte xValue = 0x01;
	constexpr word indirectAddress = 0x00FF;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr word targetAddress = 0x634F;
	constexpr byte aValue      = 0b1111'0000;
	constexpr byte memValue    = 0b1011'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = EOR_INDX.cycles;

	// A ^ mem
	cpu.setA(aValue);
	cpu.setX(xValue);
	memory[TEST_MAIN_ADDRESS] = EOR_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[0] = targetAddressMsb;
	memory[targetAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** (Indirect), Y ******************** //
TEST_F(CPUTests, eorIndYWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr byte yValue = 0x1C;
	constexpr word targetAddress = 0x636B;
	constexpr byte aValue      = 0b1111'0000;
	constexpr byte memValue    = 0b1011'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = EOR_INDY.cycles;

	// A ^ mem
	cpu.setA(aValue);
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = EOR_INDY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap & Page cross test ********** //
TEST_F(CPUTests, eorIndYFullyWrapsAndPageCrosses)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte targetAddressLsb = 0xFF;
	constexpr byte targetAddressMsb = 0xFF;
	constexpr byte yValue = 0xFF;
	constexpr word targetAddress = 0x00FE;
	constexpr byte aValue      = 0b1111'0000;
	constexpr byte memValue    = 0b1011'0010;
	constexpr byte targetValue = 0b0100'0010;
	constexpr sdword targetCycles = EOR_INDY.cycles;

	// A ^ mem
	cpu.setA(aValue);
	cpu.setY(yValue);
	memory[TEST_MAIN_ADDRESS] = EOR_INDY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromEOR(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}
