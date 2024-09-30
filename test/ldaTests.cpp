#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromLDA(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
}

// ******************** Immediate ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, ldaImmPosWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = LDA_IMM.cycles;

	// Load to A
	memory[PC_RESET] = LDA_IMM.opcode;
	memory[PC_RESET + 1] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, ldaImmNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte targetValue = 0xD8;
	constexpr sdword targetCycles = LDA_IMM.cycles;

	// Load to A, negative value
	memory[PC_RESET] = LDA_IMM.opcode;
	memory[PC_RESET + 1] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, ldaImmNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte targetValue = 0x00;
	constexpr sdword targetCycles = LDA_IMM.cycles;

	// Load to A, null value
	memory[PC_RESET] = LDA_IMM.opcode;
	memory[PC_RESET + 1] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, ldaZPPosWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = LDA_ZP.cycles;

	// Load to A
	memory[PC_RESET] = LDA_ZP.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, ldaZPNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0xF7;
	constexpr byte targetValue = 0xD8;
	constexpr sdword targetCycles = LDA_ZP.cycles;

	// Load to A, negative value
	memory[PC_RESET] = LDA_ZP.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, ldaZPNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0xF7;
	constexpr byte targetValue = 0x00;
	constexpr sdword targetCycles = LDA_ZP.cycles;

	// Load to A, null value
	memory[PC_RESET] = LDA_ZP.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page, X ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, ldaZPXPosWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0x0F;
	constexpr byte targetAddress = 0x8F;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = LDA_ZPX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[targetAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap test ********** //
TEST_F(CPUTests, ldaZPXWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0xFF;
	constexpr byte targetAddress = 0x7F;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = LDA_ZPX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[targetAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, ldaZPXNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0x0F;
	constexpr byte targetAddress = 0x8F;
	constexpr byte targetValue = 0xD8;
	constexpr sdword targetCycles = LDA_ZPX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[targetAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, ldaZPXNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0x0F;
	constexpr byte targetAddress = 0x8F;
	constexpr byte targetValue = 0x00;
	constexpr sdword targetCycles = LDA_ZPX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[targetAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, ldaAbsPosWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr word address = 0x3224;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = LDA_ABS.cycles;

	// Load to A
	memory[PC_RESET] = LDA_ABS.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, ldaAbsNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr word address = 0x3224;
	constexpr byte targetValue = 0xD8;
	constexpr sdword targetCycles = LDA_ABS.cycles;

	// Load to A
	memory[PC_RESET] = LDA_ABS.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, ldaAbsNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr word address = 0x3224;
	constexpr byte targetValue = 0x00;
	constexpr sdword targetCycles = LDA_ABS.cycles;

	// Load to A
	memory[PC_RESET] = LDA_ABS.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute, X ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, ldaAbsXPosWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = LDA_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, ldaAbsXCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0xFF;
	constexpr word address = 0x3323;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = LDA_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, ldaAbsXCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0xFF;
	constexpr byte addressMsb = 0xFF;
	constexpr byte xValue = 0x01;
	constexpr word address = 0x0000;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = LDA_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ********** Negative test ********** //
TEST_F(CPUTests, ldaAbsXNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte targetValue = 0xD8;
	constexpr sdword targetCycles = LDA_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, ldaAbsXNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte targetValue = 0x00;
	constexpr sdword targetCycles = LDA_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute, Y ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, ldaAbsYPosWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte yValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = LDA_ABSY.cycles;

	// Load to A
	cpu.setY(yValue);
	memory[PC_RESET] = LDA_ABSY.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, ldaAbsYCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte yValue = 0xFF;
	constexpr word address = 0x3323;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = LDA_ABSY.cycles;

	// Load to A
	cpu.setY(yValue);
	memory[PC_RESET] = LDA_ABSY.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, ldaAbsYCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0xFF;
	constexpr byte addressMsb = 0xFF;
	constexpr byte yValue = 0x01;
	constexpr word address = 0x0000;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = LDA_ABSY.cycles;

	// Load to A
	cpu.setY(yValue);
	memory[PC_RESET] = LDA_ABSY.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ********** Negative test ********** //
TEST_F(CPUTests, ldaAbsYNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte yValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte targetValue = 0xD8;
	constexpr sdword targetCycles = LDA_ABSY.cycles;

	// Load to A
	cpu.setY(yValue);
	memory[PC_RESET] = LDA_ABSY.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, ldaAbsYNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte yValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte targetValue = 0x00;
	constexpr sdword targetCycles = LDA_ABSY.cycles;

	// Load to A
	cpu.setY(yValue);
	memory[PC_RESET] = LDA_ABSY.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** (Indirect, X) ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, ldaIndXPosWorks)
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
	constexpr sdword targetCycles = LDA_INDX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_INDX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Full wrap test ********** //
TEST_F(CPUTests, ldaIndXFullyWraps)
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
	constexpr sdword targetCycles = LDA_INDX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_INDX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Half wrap test ********** //
TEST_F(CPUTests, ldaIndXHalfWraps)
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
	constexpr sdword targetCycles = LDA_INDX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_INDX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[0] = targetAddressMsb;
	memory[targetAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, ldaIndXNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte xValue = 0x1C;
	constexpr word indirectAddress = 0x0040;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr word targetAddress = 0x634F;
	constexpr byte targetValue = 0xD8;
	constexpr sdword targetCycles = LDA_INDX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_INDX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, ldaIndXNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte xValue = 0x1C;
	constexpr word indirectAddress = 0x0040;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr word targetAddress = 0x634F;
	constexpr byte targetValue = 0x00;
	constexpr sdword targetCycles = LDA_INDX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_INDX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	memory[targetAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** (Indirect), Y ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, ldaIndYPosWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr byte yValue = 0x1C;
	constexpr word targetAddress = 0x636B;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = LDA_INDY.cycles;

	// Load to A
	cpu.setY(yValue);
	memory[PC_RESET] = LDA_INDY.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap & Page cross test ********** //
TEST_F(CPUTests, ldaIndYFullyWrapsAndPageCrosses)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte targetAddressLsb = 0xFF;
	constexpr byte targetAddressMsb = 0xFF;
	constexpr byte yValue = 0xFF;
	constexpr word targetAddress = 0x00FE;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = LDA_INDY.cycles;

	// Load to A
	cpu.setY(yValue);
	memory[PC_RESET] = LDA_INDY.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ********** Negative test ********** //
TEST_F(CPUTests, ldaIndYNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr byte yValue = 0x1C;
	constexpr word targetAddress = 0x636B;
	constexpr byte targetValue = 0xD8;
	constexpr sdword targetCycles = LDA_INDY.cycles;

	// Load to A
	cpu.setY(yValue);
	memory[PC_RESET] = LDA_INDY.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, ldaIndYNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x63;
	constexpr byte yValue = 0x1C;
	constexpr word targetAddress = 0x636B;
	constexpr byte targetValue = 0x00;
	constexpr sdword targetCycles = LDA_INDY.cycles;

	// Load to A
	cpu.setY(yValue);
	memory[PC_RESET] = LDA_INDY.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = targetAddressLsb;
	memory[zpAddress + 1] = targetAddressMsb;
	memory[targetAddress] = targetValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
