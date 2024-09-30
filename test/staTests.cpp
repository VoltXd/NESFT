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
	constexpr byte zpAddress = 0x24;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STA_ZP.cycles;

	// Store A
	memory[PC_RESET] = STA_ZP.opcode;
	memory[PC_RESET + 1] = zpAddress;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0x0F;
	constexpr byte targetAddress = 0x8F;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STA_ZPX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[PC_RESET] = STA_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0xFF;
	constexpr byte targetAddress = 0x7F;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STA_ZPX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[PC_RESET] = STA_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr word address = 0x3224;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STA_ABS.cycles;

	// Store A
	memory[PC_RESET] = STA_ABS.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STA_ABSX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[PC_RESET] = STA_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0xFF;
	constexpr word address = 0x3323;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STA_ABSX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[PC_RESET] = STA_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte addressLsb = 0xFF;
	constexpr byte addressMsb = 0xFF;
	constexpr byte xValue = 0x01;
	constexpr word address = 0x0000;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STA_ABSX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[PC_RESET] = STA_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte yValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STA_ABSY.cycles;

	// Store A
	cpu.setY(yValue);
	memory[PC_RESET] = STA_ABSY.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte yValue = 0xFF;
	constexpr word address = 0x3323;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STA_ABSY.cycles;

	// Store A
	cpu.setY(yValue);
	memory[PC_RESET] = STA_ABSY.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte addressLsb = 0xFF;
	constexpr byte addressMsb = 0xFF;
	constexpr byte yValue = 0x01;
	constexpr word address = 0x0000;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STA_ABSY.cycles;

	// Store A
	cpu.setY(yValue);
	memory[PC_RESET] = STA_ABSY.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte zpAddress = 0x24;
	constexpr byte xValue = 0x1C;
	constexpr word indirectAddress = 0x0040;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr word targetAddress = 0x634F;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STA_INDX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[PC_RESET] = STA_INDX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte zpAddress = 0x24;
	constexpr byte xValue = 0xFF;
	constexpr word indirectAddress = 0x0023;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr word targetAddress = 0x634F;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STA_INDX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[PC_RESET] = STA_INDX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte zpAddress = 0xFE;
	constexpr byte xValue = 0x01;
	constexpr word indirectAddress = 0x00FF;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr word targetAddress = 0x634F;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STA_INDX.cycles;

	// Store A
	cpu.setX(xValue);
	memory[PC_RESET] = STA_INDX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[0] = targetAddressMsb;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte zpAddress = 0x24;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr byte yValue = 0x1C;
	constexpr word targetAddress = 0x636B;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STA_INDY.cycles;

	// Store A
	cpu.setY(yValue);
	memory[PC_RESET] = STA_INDY.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr byte zpAddress = 0x24;
	constexpr byte targetAddressLsb = 0xFF;
	constexpr byte targetAddressMsb = 0xFF;
	constexpr byte yValue = 0xFF;
	constexpr word targetAddress = 0x00FE;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = STA_INDY.cycles;

	// Store A
	cpu.setY(yValue);
	memory[PC_RESET] = STA_INDY.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	cpu.setA(targetValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromSTA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
