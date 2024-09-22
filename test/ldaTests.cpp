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
	constexpr dword targetCycles = LDA_IMM.cycles;


	// Load to A, positive value
	memory[PC_RESET] = LDA_IMM.opcode;
	memory[PC_RESET + 1] = targetValue;
	dword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr dword targetCycles = LDA_IMM.cycles;

	// Load to A, negative value
	memory[PC_RESET] = LDA_IMM.opcode;
	memory[PC_RESET + 1] = targetValue;
	dword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr dword targetCycles = LDA_IMM.cycles;

	// Load to A, null value
	memory[PC_RESET] = LDA_IMM.opcode;
	memory[PC_RESET + 1] = targetValue;
	dword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr dword targetCycles = LDA_ZP.cycles;


	// Load to A, positive value
	memory[PC_RESET] = LDA_ZP.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[ZP_START | zpAddress] = targetValue;
	dword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr dword targetCycles = LDA_ZP.cycles;

	// Load to A, negative value
	memory[PC_RESET] = LDA_ZP.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[ZP_START | zpAddress] = targetValue;
	dword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr dword targetCycles = LDA_ZP.cycles;

	// Load to A, null value
	memory[PC_RESET] = LDA_ZP.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[ZP_START | zpAddress] = targetValue;
	dword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr dword targetCycles = LDA_ZPX.cycles;

	// Load to A, positive value
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[ZP_START | targetAddress] = targetValue;
	dword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr dword targetCycles = LDA_ZPX.cycles;

	// Load to A, positive value
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[ZP_START | targetAddress] = targetValue;
	dword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr dword targetCycles = LDA_ZPX.cycles;

	// Load to A, positive value
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[ZP_START | targetAddress] = targetValue;
	dword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr dword targetCycles = LDA_ZPX.cycles;


	// Load to A, positive value
	cpu.setX(xValue);
	memory[PC_RESET] = LDA_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[ZP_START | targetAddress] = targetValue;
	dword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr dword targetCycles = LDA_ABS.cycles;


	// Load to A, positive value
	memory[PC_RESET] = LDA_ABS.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = targetValue;
	dword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr dword targetCycles = LDA_ABS.cycles;


	// Load to A, positive value
	memory[PC_RESET] = LDA_ABS.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = targetValue;
	dword elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr dword targetCycles = LDA_ABS.cycles;


	// Load to A, positive value
	memory[PC_RESET] = LDA_ABS.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = targetValue;
	dword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromLDA(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute, X ******************** //
// ******************** Absolute, Y ******************** //
// ******************** (Indirect, X) ******************** //
// ******************** (Indirect), Y ******************** //