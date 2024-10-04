#include "CPUTests.hpp"

// ************** Cross page ************** //
TEST_F(CPUTests, beqCrossesPage)
{
	// Target values
	constexpr byte relAddress = 255 - 15;
	constexpr word targetPc = TEST_MAIN_ADDRESS - 14;

	constexpr sdword targetCycles = BEQ.cycles;

	// Run program
	cpu.setZ(1);
	memory[TEST_MAIN_ADDRESS] = BEQ.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = relAddress;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetPc);
	EXPECT_EQ(elapsedCycles, targetCycles + 2);
}

// ************** Not Branch ************** //
TEST_F(CPUTests, beqDoesntBranch)
{
	// Target values
	constexpr byte relAddress = 255 - 15;
	constexpr word targetPc = TEST_MAIN_ADDRESS + 2;

	constexpr sdword targetCycles = BEQ.cycles;

	// Run program
	cpu.setZ(0);
	memory[TEST_MAIN_ADDRESS] = BEQ.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = relAddress;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetPc);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ************** Branches ************** //
TEST_F(CPUTests, beqBranches)
{
	// Target values
	constexpr byte relAddress = 0x08;
	constexpr word targetPc = 0x800A;

	constexpr sdword targetCycles = BEQ.cycles;

	// Run program
	cpu.setZ(1);
	memory[TEST_MAIN_ADDRESS] = BEQ.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = relAddress;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetPc);
	EXPECT_EQ(elapsedCycles, targetCycles + 1); 
}