#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromINC(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
}

// ******************** Zero Page ******************** //
// ********** Positive test ********** //
TEST_F(CPUTests, incZPWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte memValue = 0x41;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = INC_ZP.cycles;

	// Load to A
	memory[PC_RESET] = INC_ZP.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[zpAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Negative test ********** //
TEST_F(CPUTests, incZPNegWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte memValue = 0xD5;
	constexpr byte targetValue = 0xD6;
	constexpr sdword targetCycles = INC_ZP.cycles;

	// Load to A, negative value
	memory[PC_RESET] = INC_ZP.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[zpAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Null test ********** //
TEST_F(CPUTests, incZPNullWorks)
{	
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x24;
	constexpr byte memValue = 0xFF;
	constexpr byte targetValue = 0x00;
	constexpr sdword targetCycles = INC_ZP.cycles;

	// Load to A, null value
	memory[PC_RESET] = INC_ZP.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[zpAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[zpAddress], targetValue);
	EXPECT_TRUE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Zero Page, X ******************** //
TEST_F(CPUTests, incZPXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0x0F;
	constexpr byte targetAddress = 0x8F;
	constexpr byte memValue = 0x41;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = INC_ZPX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = INC_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[targetAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Wrap test ********** //
TEST_F(CPUTests, incZPXWraps)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte zpAddress = 0x80;
	constexpr byte xValue = 0xFF;
	constexpr byte targetAddress = 0x7F;
	constexpr byte memValue = 0x41;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = INC_ZPX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = INC_ZPX.opcode;
	memory[PC_RESET + 1] = zpAddress;
	memory[targetAddress] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[targetAddress], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, incAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr word address = 0x3224;
	constexpr byte memValue = 0x41;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = INC_ABS.cycles;

	// Load to A
	memory[PC_RESET] = INC_ABS.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Absolute, X ******************** //
TEST_F(CPUTests, incAbsXWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0x17;
	constexpr word address = 0x323B;
	constexpr byte memValue = 0x41;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = INC_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = INC_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Next Page test ********** //
TEST_F(CPUTests, incAbsXCanGoToNextPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr byte xValue = 0xFF;
	constexpr word address = 0x3323;
	constexpr byte memValue = 0x41;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = INC_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = INC_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ********** Overflow to Zero Page test ********** //
TEST_F(CPUTests, incAbsXCanOverflowToZeroPage)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0xFF;
	constexpr byte addressMsb = 0xFF;
	constexpr byte xValue = 0x01;
	constexpr word address = 0x0000;
	constexpr byte memValue = 0x41;
	constexpr byte targetValue = 0x42;
	constexpr sdword targetCycles = INC_ABSX.cycles;

	// Load to A
	cpu.setX(xValue);
	memory[PC_RESET] = INC_ABSX.opcode;
	memory[PC_RESET + 1] = addressLsb;
	memory[PC_RESET + 2] = addressMsb;
	memory[address] = memValue;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(memory[address], targetValue);
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getN());
	verifyUnmodifiedStatusFlagsFromINC(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}
