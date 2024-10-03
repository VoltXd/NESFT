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
	constexpr byte aValue = 0x42;
	constexpr byte memValue = 0x12;
	constexpr sdword targetCycles = CMP_IMM.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_IMM.opcode;
	memory[PC_RESET + 1] = memValue;
	cpu.setA(aValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte aValue = 0x42;
	constexpr byte memValue = 0x42;
	constexpr sdword targetCycles = CMP_IMM.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_IMM.opcode;
	memory[PC_RESET + 1] = memValue;
	cpu.setA(aValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte aValue = 0x41;
	constexpr byte memValue = 0x43;
	constexpr sdword targetCycles = CMP_IMM.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_IMM.opcode;
	memory[PC_RESET + 1] = memValue;
	cpu.setA(aValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte aValue = 0xFF;
	constexpr byte memValue = 0x12;
	constexpr sdword targetCycles = CMP_IMM.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_IMM.opcode;
	memory[PC_RESET + 1] = memValue;
	cpu.setA(aValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte zpAddress = 0x24;
	constexpr byte memValue = 0x12;
	constexpr byte aValue = 0x42;
	constexpr sdword targetCycles = CMP_ZP.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_ZP.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = memValue;
	cpu.setA(aValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0x0F;
	constexpr byte targetAddress = 0x8F;
	constexpr byte memValue = 0x12;
	constexpr byte aValue = 0x42;
	constexpr sdword targetCycles = CMP_ZPX.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0xFF;
	constexpr byte targetAddress = 0x7F;
	constexpr byte memValue = 0x12;
	constexpr byte aValue = 0x42;
	constexpr sdword targetCycles = CMP_ZPX.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr word address = 0x3224;
	constexpr byte memValue = 0x12;
	constexpr byte aValue = 0x42;
	constexpr sdword targetCycles = CMP_ABS.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_ABS.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte memValue = 0x12;
	constexpr byte aValue = 0x42;
	constexpr sdword targetCycles = CMP_ABSX.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0xFF;
	constexpr word address = 0x3323;
	constexpr byte memValue = 0x12;
	constexpr byte aValue = 0x42;
	constexpr sdword targetCycles = CMP_ABSX.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte addressLsb = 0xFF;
	constexpr byte addressMsb = 0xFF;
	constexpr byte xValue = 0x01;
	constexpr word address = 0x0000;
	constexpr byte memValue = 0x12;
	constexpr byte aValue = 0x42;
	constexpr sdword targetCycles = CMP_ABSX.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte yValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte memValue = 0x12;
	constexpr byte aValue = 0x42;
	constexpr sdword targetCycles = CMP_ABSY.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_ABSY.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setY(yValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte yValue = 0xFF;
	constexpr word address = 0x3323;
	constexpr byte memValue = 0x12;
	constexpr byte aValue = 0x42;
	constexpr sdword targetCycles = CMP_ABSY.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_ABSY.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setY(yValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte addressLsb = 0xFF;
	constexpr byte addressMsb = 0xFF;
	constexpr byte yValue = 0x01;
	constexpr word address = 0x0000;
	constexpr byte memValue = 0x12;
	constexpr byte aValue = 0x42;
	constexpr sdword targetCycles = CMP_ABSY.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_ABSY.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	cpu.setA(aValue);
	cpu.setY(yValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte zpAddress = 0x24;
	constexpr byte xValue = 0x1C;
	constexpr word indirectAddress = 0x0040;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr word targetAddress = 0x634F;
	constexpr byte memValue = 0x12;
	constexpr byte aValue = 0x42;
	constexpr sdword targetCycles = CMP_INDX.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_INDX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte zpAddress = 0x24;
	constexpr byte xValue = 0xFF;
	constexpr word indirectAddress = 0x0023;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr word targetAddress = 0x634F;
	constexpr byte memValue = 0x12;
	constexpr byte aValue = 0x42;
	constexpr sdword targetCycles = CMP_INDX.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_INDX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte zpAddress = 0xFE;
	constexpr byte xValue = 0x01;
	constexpr word indirectAddress = 0x00FF;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr word targetAddress = 0x634F;
	constexpr byte memValue = 0x12;
	constexpr byte aValue = 0x42;
	constexpr sdword targetCycles = CMP_INDX.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_INDX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[0] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setX(xValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte zpAddress = 0x24;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr byte yValue = 0x1C;
	constexpr word targetAddress = 0x636B;
	constexpr byte memValue = 0x12;
	constexpr byte aValue = 0x42;
	constexpr sdword targetCycles = CMP_INDY.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_INDY.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setY(yValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte zpAddress = 0x24;
	constexpr byte targetAddressLsb = 0xFF;
	constexpr byte targetAddressMsb = 0xFF;
	constexpr byte yValue = 0xFF;
	constexpr word targetAddress = 0x00FE;
	constexpr byte memValue = 0x12;
	constexpr byte aValue = 0x42;
	constexpr sdword targetCycles = CMP_INDY.cycles;

	// Set up CPU & execute
	memory[PC_RESET] = CMP_INDY.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = memValue;
	cpu.setA(aValue);
	cpu.setY(yValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_TRUE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromCMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}
