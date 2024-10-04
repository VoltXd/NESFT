#include "CPUTests.hpp"

// ************** Cross page ************** //
TEST_F(CPUTests, bcsCrossesPage)
{
	// Target values
	constexpr byte relAddress = 255 - 15;
	constexpr word targetPc = TEST_MAIN_ADDRESS - 14;

	constexpr sdword targetCycles = BCS.cycles;

	// Run program
	cpu.setC(1);
	memory[TEST_MAIN_ADDRESS] = BCS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = relAddress;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetPc);
	EXPECT_EQ(elapsedCycles, targetCycles + 2);
}

// ************** Not Branch ************** //
TEST_F(CPUTests, bcsDoesntBranch)
{
	// Target values
	constexpr byte relAddress = 255 - 15;
	constexpr word targetPc = TEST_MAIN_ADDRESS + 2;

	constexpr sdword targetCycles = BCS.cycles;

	// Run program
	cpu.setC(0);
	memory[TEST_MAIN_ADDRESS] = BCS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = relAddress;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetPc);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ************** Branches ************** //
TEST_F(CPUTests, bcsBranches)
{
	// Target values
	constexpr byte relAddress = 0x08;
	constexpr word targetPc = 0x800A;

	constexpr sdword targetCycles = BCS.cycles;

	// Run program
	cpu.setC(1);
	memory[TEST_MAIN_ADDRESS] = BCS.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = relAddress;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetPc);
	EXPECT_EQ(elapsedCycles, targetCycles + 1); 
}