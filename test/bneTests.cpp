#include "CPUTests.hpp"

// ************** Branches ************** //
TEST_F(CPUTests, bneBranches)
{
	// Target values
	constexpr byte relAddress = 255 - 15;
	constexpr word targetPc = 0xFFFC - 14;

	constexpr sdword targetCycles = BNE.cycles;

	// Run program
	memory[PC_RESET] = BNE.opcode;
	memory[PC_RESET + 1] = relAddress;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetPc);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ************** Not Branch ************** //
TEST_F(CPUTests, bneDoesntBranch)
{
	// Target values
	constexpr byte relAddress = 255 - 15;
	constexpr word targetPc = 0xFFFC + 2;

	constexpr sdword targetCycles = BNE.cycles;

	// Run program
	cpu.setZ(1);
	memory[PC_RESET] = BNE.opcode;
	memory[PC_RESET + 1] = relAddress;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetPc);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ************** Cross page ************** //
TEST_F(CPUTests, bneCrossesPage)
{
	// Target values
	constexpr byte relAddress = 0x08;
	constexpr word targetPc = 0x0006;

	constexpr sdword targetCycles = BNE.cycles;

	// Run program
	memory[PC_RESET] = BNE.opcode;
	memory[PC_RESET + 1] = relAddress;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetPc);
	EXPECT_EQ(elapsedCycles, targetCycles + 2); 
}