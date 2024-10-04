#include "CPUTests.hpp"

// ************** Branches ************** //
TEST_F(CPUTests, bccBranches)
{
	// Target values
	constexpr byte relAddress = 255 - 15;
	constexpr word targetPc = 0xFFFC - 14;

	constexpr sdword targetCycles = BCC.cycles;

	// Run program
	memory[PC_RESET] = BCC.opcode;
	memory[PC_RESET + 1] = relAddress;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetPc);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ************** Not Branch ************** //
TEST_F(CPUTests, bccDoesntBranch)
{
	// Target values
	constexpr byte relAddress = 255 - 15;
	constexpr word targetPc = 0xFFFC + 2;

	constexpr sdword targetCycles = BCC.cycles;

	// Run program
	cpu.setC(1);
	memory[PC_RESET] = BCC.opcode;
	memory[PC_RESET + 1] = relAddress;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetPc);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ************** Cross page ************** //
TEST_F(CPUTests, bccCrossesPage)
{
	// Target values
	constexpr byte relAddress = 0x08;
	constexpr word targetPc = 0x0006;

	constexpr sdword targetCycles = BCC.cycles;

	// Run program
	memory[PC_RESET] = BCC.opcode;
	memory[PC_RESET + 1] = relAddress;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetPc);
	EXPECT_EQ(elapsedCycles, targetCycles + 2); 
}