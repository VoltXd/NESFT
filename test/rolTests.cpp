#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromROL(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
}

// ******************** Accumulator ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, rolAccWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte inputValue = 0b0001'1001;
	constexpr byte targetValue = 0b0011'0011;
	constexpr sdword targetCycles = ROL_ACC.cycles;

	// Load to A
	memory[PC_RESET] = ROL_ACC.opcode;
	cpu.setA(inputValue);
	cpu.setC(1);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, rolAccNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte inputValue = 0b0101'1001;
	constexpr byte targetValue = 0b1011'0010;
	constexpr sdword targetCycles = ROL_ACC.cycles;

	// Load to A
	memory[PC_RESET] = ROL_ACC.opcode;
	cpu.setA(inputValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null & carry test ********** //
TEST_F(CPUTests, rolAccNullCarryWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte inputValue = 0b1000'0000;
	constexpr byte targetValue = 0b0000'0000;
	constexpr sdword targetCycles = ROL_ACC.cycles;

	// Load to A
	memory[PC_RESET] = ROL_ACC.opcode;
	cpu.setA(inputValue);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getA(), targetValue);
	EXPECT_TRUE(cpu.getC());
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page ******************** //
TEST_F(CPUTests, rolZPWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte inputValue = 0b0001'1001;
	constexpr byte targetValue = 0b0011'0011;
	constexpr sdword targetCycles = ROL_ZP.cycles;

	// Load to A
	memory[PC_RESET] = ROL_ZP.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = inputValue;
	cpu.setC(1);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[zpAddress], targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page, X ******************** //
TEST_F(CPUTests, rolZPXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0x0F;
	constexpr byte targetAddress = 0x8F;
	constexpr byte inputValue = 0b0001'1001;
	constexpr byte targetValue = 0b0011'0011;
	constexpr sdword targetCycles = ROL_ZPX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = ROL_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[targetAddress] = inputValue;
	cpu.setC(1);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap test ********** //
TEST_F(CPUTests, rolZPXWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0xFF;
	constexpr byte targetAddress = 0x7F;
	constexpr byte inputValue = 0b0001'1001;
	constexpr byte targetValue = 0b0011'0011;
	constexpr sdword targetCycles = ROL_ZPX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = ROL_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[targetAddress] = inputValue;
	cpu.setC(1);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, rolAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr word address = 0x3224;
	constexpr byte inputValue = 0b0001'1001;
	constexpr byte targetValue = 0b0011'0011;
	constexpr sdword targetCycles = ROL_ABS.cycles;

	// Load to A
	memory[PC_RESET] = ROL_ABS.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = inputValue;
	cpu.setC(1);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute, X ******************** //
TEST_F(CPUTests, rolAbsXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte inputValue = 0b0001'1001;
	constexpr byte targetValue = 0b0011'0011;
	constexpr sdword targetCycles = ROL_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = ROL_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = inputValue;
	cpu.setC(1);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, rolAbsXCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0xFF;
	constexpr word address = 0x3323;
	constexpr byte inputValue = 0b0001'1001;
	constexpr byte targetValue = 0b0011'0011;
	constexpr sdword targetCycles = ROL_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = ROL_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = inputValue;
	cpu.setC(1);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, rolAbsXCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0xFF;
	constexpr byte addressMsb = 0xFF;
	constexpr byte xValue = 0x01;
	constexpr word address = 0x0000;
	constexpr byte inputValue = 0b0001'1001;
	constexpr byte targetValue = 0b0011'0011;
	constexpr sdword targetCycles = ROL_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = ROL_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = inputValue;
	cpu.setC(1);
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromROL(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
