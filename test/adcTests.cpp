#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromADC(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
}

// ******************** Immediate ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, adcImmPosWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte memValue = 0x42;
	constexpr byte aValue = 0x12;
	constexpr byte cValue = 1;
	constexpr byte targetValue = 0x55;
	constexpr sdword targetCycles = ADC_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative & overflow & carry test ********** //
TEST_F(CPUTests, adcImmNegOveCarWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte memValue = 0xFF;
	constexpr byte aValue = 0x80;
	constexpr byte cValue = 0;
	constexpr byte targetValue = 0x7F;
	constexpr sdword targetCycles = ADC_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** 2nd overflow test ********** //
TEST_F(CPUTests, adcImmOveWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte memValue = 0x7F;
	constexpr byte aValue = 0x00;
	constexpr byte cValue = 1;
	constexpr byte targetValue = 0x80;
	constexpr sdword targetCycles = ADC_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getV());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, adcImmNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte memValue = 0x00;
	constexpr byte aValue = 0x00;
	constexpr byte cValue = 0;
	constexpr byte targetValue = 0x00;
	constexpr sdword targetCycles = ADC_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page ******************** //
TEST_F(CPUTests, adcZPWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte memValue = 0x42;
	constexpr byte aValue = 0x12;
	constexpr byte cValue = 1;
	constexpr byte targetValue = 0x55;
	constexpr sdword targetCycles = ADC_ZP.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page, X ******************** //
TEST_F(CPUTests, adcZPXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0x0F;
	constexpr byte targetAddress = 0x8F;
	constexpr byte memValue = 0x42;
	constexpr byte aValue = 0x12;
	constexpr byte cValue = 1;
	constexpr byte targetValue = 0x55;
	constexpr sdword targetCycles = ADC_ZPX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap test ********** //
TEST_F(CPUTests, adcZPXWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0xFF;
	constexpr byte targetAddress = 0x7F;
	constexpr byte memValue = 0x42;
	constexpr byte aValue = 0x12;
	constexpr byte cValue = 1;
	constexpr byte targetValue = 0x55;
	constexpr sdword targetCycles = ADC_ZPX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, adcAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr word address = 0x3224;
	constexpr byte memValue = 0x42;
	constexpr byte aValue = 0x12;
	constexpr byte cValue = 1;
	constexpr byte targetValue = 0x55;
	constexpr sdword targetCycles = ADC_ABS.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ABS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute, X ******************** //
TEST_F(CPUTests, adcAbsXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte memValue = 0x42;
	constexpr byte aValue = 0x12;
	constexpr byte cValue = 1;
	constexpr byte targetValue = 0x55;
	constexpr sdword targetCycles = ADC_ABSX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, adcAbsXCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0xFF;
	constexpr word address = 0x3323;
	constexpr byte memValue = 0x42;
	constexpr byte aValue = 0x12;
	constexpr byte cValue = 1;
	constexpr byte targetValue = 0x55;
	constexpr sdword targetCycles = ADC_ABSX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, adcAbsXCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0xFF;
	constexpr byte addressMsb = 0xFF;
	constexpr byte xValue = 0x01;
	constexpr word address = 0x0000;
	constexpr byte memValue = 0x42;
	constexpr byte aValue = 0x12;
	constexpr byte cValue = 1;
	constexpr byte targetValue = 0x55;
	constexpr sdword targetCycles = ADC_ABSX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ******************** Absolute, Y ******************** //
TEST_F(CPUTests, adcAbsYWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte yValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte memValue = 0x42;
	constexpr byte aValue = 0x12;
	constexpr byte cValue = 1;
	constexpr byte targetValue = 0x55;
	constexpr sdword targetCycles = ADC_ABSY.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setY(yValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, adcAbsYCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte yValue = 0xFF;
	constexpr word address = 0x3323;
	constexpr byte memValue = 0x42;
	constexpr byte aValue = 0x12;
	constexpr byte cValue = 1;
	constexpr byte targetValue = 0x55;
	constexpr sdword targetCycles = ADC_ABSY.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setY(yValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, adcAbsYCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0xFF;
	constexpr byte addressMsb = 0xFF;
	constexpr byte yValue = 0x01;
	constexpr word address = 0x0000;
	constexpr byte memValue = 0x42;
	constexpr byte aValue = 0x12;
	constexpr byte cValue = 1;
	constexpr byte targetValue = 0x55;
	constexpr sdword targetCycles = ADC_ABSY.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setY(yValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ******************** (Indirect, X) ******************** //
TEST_F(CPUTests, adcIndXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte xValue = 0x1C;
	constexpr word indirectAddress = 0x0040;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr word targetAddress = 0x634F;
	constexpr byte memValue = 0x42;
	constexpr byte aValue = 0x12;
	constexpr byte cValue = 1;
	constexpr byte targetValue = 0x55;
	constexpr sdword targetCycles = ADC_INDX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Full wrap test ********** //
TEST_F(CPUTests, adcIndXFullyWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte xValue = 0xFF;
	constexpr word indirectAddress = 0x0023;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr word targetAddress = 0x634F;
	constexpr byte memValue = 0x42;
	constexpr byte aValue = 0x12;
	constexpr byte cValue = 1;
	constexpr byte targetValue = 0x55;
	constexpr sdword targetCycles = ADC_INDX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Half wrap test ********** //
TEST_F(CPUTests, adcIndXHalfWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0xFE;
	constexpr byte xValue = 0x01;
	constexpr word indirectAddress = 0x00FF;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr word targetAddress = 0x634F;
	constexpr byte memValue = 0x42;
	constexpr byte aValue = 0x12;
	constexpr byte cValue = 1;
	constexpr byte targetValue = 0x55;
	constexpr sdword targetCycles = ADC_INDX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[0] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** (Indirect), Y ******************** //
TEST_F(CPUTests, adcIndYWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr byte yValue = 0x1C;
	constexpr word targetAddress = 0x636B;
	constexpr byte memValue = 0x42;
	constexpr byte aValue = 0x12;
	constexpr byte cValue = 1;
	constexpr byte targetValue = 0x55;
	constexpr sdword targetCycles = ADC_INDY.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_INDY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setY(yValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap & Page cross test ********** //
TEST_F(CPUTests, adcIndYFullyWrapsAndPageCrosses)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte targetAddressLsb = 0xFF;
	constexpr byte targetAddressMsb = 0xFF;
	constexpr byte yValue = 0xFF;
	constexpr word targetAddress = 0x00FE;
	constexpr byte memValue = 0x42;
	constexpr byte aValue = 0x12;
	constexpr byte cValue = 1;
	constexpr byte targetValue = 0x55;
	constexpr sdword targetCycles = ADC_INDY.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_INDY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setY(yValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}
