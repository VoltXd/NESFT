#include "CPUTests.hpp"

TEST_F(CPUTests, cpuDoesNothingWhenWeExecuteZeroCycles)
{
	// Target values
	constexpr sdword targetCycles = 0;

	// Execute 0 cycles
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), PC_RESET);
	EXPECT_EQ(cpu.getSp(), SP_RESET);

	EXPECT_EQ(cpu.getA(), 0);
	EXPECT_EQ(cpu.getX(), 0);
	EXPECT_EQ(cpu.getY(), 0);
	
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getI());
	EXPECT_FALSE(cpu.getD());
	EXPECT_FALSE(cpu.getB());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());

	EXPECT_EQ(elapsedCycles, targetCycles);
}

TEST_F(CPUTests, cpuCanExecuteMoreCyclesThanRequested)
{
	// Target values
	constexpr byte targetvalue = 0x42;
	constexpr sdword targetCycles = LDA_IMM.cycles;
	constexpr sdword cyclesToExecute = 1;

	// Execute 1 cycles
	memory[PC_RESET] = LDA_IMM.opcode;
	memory[PC_RESET + 1] = targetvalue;
	sdword elapsedCycles = cpu.execute(cyclesToExecute, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), PC_RESET + 2);
	EXPECT_EQ(cpu.getSp(), SP_RESET);

	EXPECT_EQ(cpu.getA(), targetvalue);
	EXPECT_EQ(cpu.getX(), 0);
	EXPECT_EQ(cpu.getY(), 0);
	
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_FALSE(cpu.getI());
	EXPECT_FALSE(cpu.getD());
	EXPECT_FALSE(cpu.getB());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());

	EXPECT_EQ(elapsedCycles, targetCycles);
}