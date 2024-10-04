#include "CPUTests.hpp"

static void verifyUnmodifiedStatusFlagsFromJMP(const CPU& cpu, const CPU& cpuInitialState)
{
	EXPECT_EQ(cpu.getC(), cpuInitialState.getC());
	EXPECT_EQ(cpu.getZ(), cpuInitialState.getZ());
	EXPECT_EQ(cpu.getI(), cpuInitialState.getI());
	EXPECT_EQ(cpu.getD(), cpuInitialState.getD());
	EXPECT_EQ(cpu.getB(), cpuInitialState.getB());
	EXPECT_EQ(cpu.getV(), cpuInitialState.getV());
	EXPECT_EQ(cpu.getN(), cpuInitialState.getN());
}

// ******************** Absolute ******************** //
TEST_F(CPUTests, jmpAbsWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte addressLsb = 0x24;
	constexpr byte addressMsb = 0x32;
	constexpr word address = 0x3224;
	constexpr sdword targetCycles = JMP_ABS.cycles;

	// Test bits
	memory[TEST_MAIN_ADDRESS] = JMP_ABS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), address);
	verifyUnmodifiedStatusFlagsFromJMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ******************** Indirect ******************** //
TEST_F(CPUTests, jmpIndWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte indirectAddressLsb = 0x24;
	constexpr byte indirectAddressMsb = 0x32;
	constexpr word indirectAddress = 0x3224;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x87;
	constexpr word targetAddress = 0x874F;
	constexpr sdword targetCycles = JMP_IND.cycles;

	// Test bits
	memory[TEST_MAIN_ADDRESS] = JMP_IND.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = indirectAddressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = indirectAddressMsb;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetAddress);
	verifyUnmodifiedStatusFlagsFromJMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

TEST_F(CPUTests, jmpIndWrapWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr byte indirectAddressLsb = 0xFF;
	constexpr byte indirectAddressMsb = 0x32;
	constexpr word indirectAddress = 0x32FF;
	constexpr byte targetAddressLsb = 0x4F;
	constexpr byte targetAddressMsb = 0x87;
	constexpr word targetAddress = 0x874F;
	constexpr sdword targetCycles = JMP_IND.cycles;

	// Test bits
	memory[TEST_MAIN_ADDRESS] = JMP_IND.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = indirectAddressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = indirectAddressMsb;
	memory[indirectAddress] = targetAddressLsb;
	memory[0x3200] = targetAddressMsb;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetAddress);
	verifyUnmodifiedStatusFlagsFromJMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}


