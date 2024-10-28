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
	constexpr u8 addressLsb = 0x24;
	constexpr u8 addressMsb = 0x32;
	constexpr u16 address = 0x3224;
	constexpr s32 targetCycles = JMP_ABS.cycles;

	// Test bits
	memory[TEST_MAIN_ADDRESS] = JMP_ABS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = addressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = addressMsb;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

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
	constexpr u8 indirectAddressLsb = 0x24;
	constexpr u8 indirectAddressMsb = 0x32;
	constexpr u16 indirectAddress = 0x3224;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x87;
	constexpr u16 targetAddress = 0x874F;
	constexpr s32 targetCycles = JMP_IND.cycles;

	// Test bits
	memory[TEST_MAIN_ADDRESS] = JMP_IND.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = indirectAddressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = indirectAddressMsb;
	memory[indirectAddress] = targetAddressLsb;
	memory[indirectAddress + 1] = targetAddressMsb;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetAddress);
	verifyUnmodifiedStatusFlagsFromJMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

TEST_F(CPUTests, jmpIndWrapWorks)
{
	// Target values
	const CPU cpuInitialState = cpu;
	constexpr u8 indirectAddressLsb = 0xFF;
	constexpr u8 indirectAddressMsb = 0x32;
	constexpr u16 indirectAddress = 0x32FF;
	constexpr u8 targetAddressLsb = 0x4F;
	constexpr u8 targetAddressMsb = 0x87;
	constexpr u16 targetAddress = 0x874F;
	constexpr s32 targetCycles = JMP_IND.cycles;

	// Test bits
	memory[TEST_MAIN_ADDRESS] = JMP_IND.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = indirectAddressLsb;
	memory[TEST_MAIN_ADDRESS + 2] = indirectAddressMsb;
	memory[indirectAddress] = targetAddressLsb;
	memory[0x3200] = targetAddressMsb;
	s32 elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetAddress);
	verifyUnmodifiedStatusFlagsFromJMP(cpu, cpuInitialState);
	EXPECT_EQ(elapsedCycles, targetCycles);
}


