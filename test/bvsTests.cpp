#include "CPUTests.hpp"

// ************** Branches ************** //
TEST_F(CPUTests, bvsBranches)
{
	// Target values
	constexpr byte relAddress = 255 - 15;
	constexpr word targetPc = 0xFFFC - 14;

	constexpr sdword targetCycles = BVS.cycles;

	// Run program
	cpu.setV(1);
	memory[PC_RESET] = BVS.opcode;
	memory[PC_RESET + 1] = relAddress;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetPc);
	EXPECT_EQ(elapsedCycles, targetCycles + 1);
}

// ************** Not Branch ************** //
TEST_F(CPUTests, bvsDoesntBranch)
{
	// Target values
	constexpr byte relAddress = 255 - 15;
	constexpr word targetPc = 0xFFFC + 2;

	constexpr sdword targetCycles = BVS.cycles;

	// Run program
	cpu.setV(0);
	memory[PC_RESET] = BVS.opcode;
	memory[PC_RESET + 1] = relAddress;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetPc);
	EXPECT_EQ(elapsedCycles, targetCycles);
}

// ************** Cross page ************** //
TEST_F(CPUTests, bvsCrossesPage)
{
	// Target values
	constexpr byte relAddress = 0x08;
	constexpr word targetPc = 0x0006;

	constexpr sdword targetCycles = BVS.cycles;

	// Run program
	cpu.setV(1);
	memory[PC_RESET] = BVS.opcode;
	memory[PC_RESET + 1] = relAddress;
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), targetPc);
	EXPECT_EQ(elapsedCycles, targetCycles + 2); 
}