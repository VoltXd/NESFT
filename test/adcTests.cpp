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
	constexpr u8 memValue = 0x42;
	constexpr u8 aValue = 0x12;
	constexpr u8 cValue = 1;
	constexpr u8 targetValue = 0x55;
	constexpr s32 targetCycles = ADC_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 memValue = 0xFF;
	constexpr u8 aValue = 0x80;
	constexpr u8 cValue = 0;
	constexpr u8 targetValue = 0x7F;
	constexpr s32 targetCycles = ADC_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 memValue = 0x7F;
	constexpr u8 aValue = 0x00;
	constexpr u8 cValue = 1;
	constexpr u8 targetValue = 0x80;
	constexpr s32 targetCycles = ADC_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 memValue = 0x00;
	constexpr u8 aValue = 0x00;
	constexpr u8 cValue = 0;
	constexpr u8 targetValue = 0x00;
	constexpr s32 targetCycles = ADC_IMM.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 zpAddress = 0x24;
	constexpr u8 memValue = 0x42;
	constexpr u8 aValue = 0x12;
	constexpr u8 cValue = 1;
	constexpr u8 targetValue = 0x55;
	constexpr s32 targetCycles = ADC_ZP.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ZP.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 zpAddress = 0x80;
	constexpr u8 xValue = 0x0F;
	constexpr u8 targetAddress = 0x8F;
	constexpr u8 memValue = 0x42;
	constexpr u8 aValue = 0x12;
	constexpr u8 cValue = 1;
	constexpr u8 targetValue = 0x55;
	constexpr s32 targetCycles = ADC_ZPX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 zpAddress = 0x80;
	constexpr u8 xValue = 0xFF;
	constexpr u8 targetAddress = 0x7F;
	constexpr u8 memValue = 0x42;
	constexpr u8 aValue = 0x12;
	constexpr u8 cValue = 1;
	constexpr u8 targetValue = 0x55;
	constexpr s32 targetCycles = ADC_ZPX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ZPX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u16 address = 0x3224;
	constexpr u8 memValue = 0x42;
	constexpr u8 aValue = 0x12;
	constexpr u8 cValue = 1;
	constexpr u8 targetValue = 0x55;
	constexpr s32 targetCycles = ADC_ABS.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ABS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 xValue = 0x17;
	constexpr u16 address = 0x323B;
	constexpr u8 memValue = 0x42;
	constexpr u8 aValue = 0x12;
	constexpr u8 cValue = 1;
	constexpr u8 targetValue = 0x55;
	constexpr s32 targetCycles = ADC_ABSX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 xValue = 0xFF;
	constexpr u16 address = 0x3323;
	constexpr u8 memValue = 0x42;
	constexpr u8 aValue = 0x12;
	constexpr u8 cValue = 1;
	constexpr u8 targetValue = 0x55;
	constexpr s32 targetCycles = ADC_ABSX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 addressLsb = 0xFF;
	constexpr u8 addressMsb = 0xFF;
	constexpr u8 xValue = 0x01;
	constexpr u16 address = 0x0000;
	constexpr u8 memValue = 0x42;
	constexpr u8 aValue = 0x12;
	constexpr u8 cValue = 1;
	constexpr u8 targetValue = 0x55;
	constexpr s32 targetCycles = ADC_ABSX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ABSX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 yValue = 0x17;
	constexpr u16 address = 0x323B;
	constexpr u8 memValue = 0x42;
	constexpr u8 aValue = 0x12;
	constexpr u8 cValue = 1;
	constexpr u8 targetValue = 0x55;
	constexpr s32 targetCycles = ADC_ABSY.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setY(yValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u8 yValue = 0xFF;
	constexpr u16 address = 0x3323;
	constexpr u8 memValue = 0x42;
	constexpr u8 aValue = 0x12;
	constexpr u8 cValue = 1;
	constexpr u8 targetValue = 0x55;
	constexpr s32 targetCycles = ADC_ABSY.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setY(yValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 addressLsb = 0xFF;
	constexpr u8 addressMsb = 0xFF;
	constexpr u8 yValue = 0x01;
	constexpr u16 address = 0x0000;
	constexpr u8 memValue = 0x42;
	constexpr u8 aValue = 0x12;
	constexpr u8 cValue = 1;
	constexpr u8 targetValue = 0x55;
	constexpr s32 targetCycles = ADC_ABSY.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_ABSY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setY(yValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 zpAddress = 0x24;
	constexpr u8 xValue = 0x1C;
	constexpr u16 indirectAddress = 0x0040;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x63;
	constexpr u16 targetAddress = 0x634F;
	constexpr u8 memValue = 0x42;
	constexpr u8 aValue = 0x12;
	constexpr u8 cValue = 1;
	constexpr u8 targetValue = 0x55;
	constexpr s32 targetCycles = ADC_INDX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 zpAddress = 0x24;
	constexpr u8 xValue = 0xFF;
	constexpr u16 indirectAddress = 0x0023;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x63;
	constexpr u16 targetAddress = 0x634F;
	constexpr u8 memValue = 0x42;
	constexpr u8 aValue = 0x12;
	constexpr u8 cValue = 1;
	constexpr u8 targetValue = 0x55;
	constexpr s32 targetCycles = ADC_INDX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 zpAddress = 0xFE;
	constexpr u8 xValue = 0x01;
	constexpr u16 indirectAddress = 0x00FF;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x63;
	constexpr u16 targetAddress = 0x634F;
	constexpr u8 memValue = 0x42;
	constexpr u8 aValue = 0x12;
	constexpr u8 cValue = 1;
	constexpr u8 targetValue = 0x55;
	constexpr s32 targetCycles = ADC_INDX.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_INDX.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[0] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setX(xValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 zpAddress = 0x24;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x63;
	constexpr u8 yValue = 0x1C;
	constexpr u16 targetAddress = 0x636B;
	constexpr u8 memValue = 0x42;
	constexpr u8 aValue = 0x12;
	constexpr u8 cValue = 1;
	constexpr u8 targetValue = 0x55;
	constexpr s32 targetCycles = ADC_INDY.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_INDY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setY(yValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 zpAddress = 0x24;
	constexpr u8 targetAddressLsb = 0xFF;
	constexpr u8 targetAddressMsb = 0xFF;
	constexpr u8 yValue = 0xFF;
	constexpr u16 targetAddress = 0x00FE;
	constexpr u8 memValue = 0x42;
	constexpr u8 aValue = 0x12;
	constexpr u8 cValue = 1;
	constexpr u8 targetValue = 0x55;
	constexpr s32 targetCycles = ADC_INDY.cycles;

	// Set up CPU & execute
	memory[TEST_MAIN_ADDRESS] = ADC_INDY.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setC(cValue);
	cpu.setY(yValue);
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromADC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}
