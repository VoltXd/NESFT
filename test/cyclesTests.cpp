#include "CPUTests.hpp"

TEST_F(CPUTests, cpuDoesNothingWhenWeExecuteZeroCycles)
{
	// Target values
	constexpr sdword targetCycles = 0;

	// Execute 0 cycles
	sdword elapsedCycles = cpu.execute(targetCycles, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), TEST_MAIN_ADDRESS);
	EXPECT_EQ(cpu.getSp(), SP_RESET - 3);

	EXPECT_EQ(cpu.getA(), 0);
	EXPECT_EQ(cpu.getX(), 0);
	EXPECT_EQ(cpu.getY(), 0);
	
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getI());
	EXPECT_FALSE(cpu.getD());
	EXPECT_TRUE(cpu.getB());
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
	memory[TEST_MAIN_ADDRESS] = LDA_IMM.opcode;
	memory[TEST_MAIN_ADDRESS + 1] = targetvalue;
	sdword elapsedCycles = cpu.execute(cyclesToExecute, memory);

	// Verify
	EXPECT_EQ(cpu.getPc(), TEST_MAIN_ADDRESS + 2);
	EXPECT_EQ(cpu.getSp(), SP_RESET - 3);

	EXPECT_EQ(cpu.getA(), targetvalue);
	EXPECT_EQ(cpu.getX(), 0);
	EXPECT_EQ(cpu.getY(), 0);
	
	EXPECT_FALSE(cpu.getC());
	EXPECT_FALSE(cpu.getZ());
	EXPECT_TRUE(cpu.getI());
	EXPECT_FALSE(cpu.getD());
	EXPECT_TRUE(cpu.getB());
	EXPECT_FALSE(cpu.getV());
	EXPECT_FALSE(cpu.getN());

	EXPECT_EQ(elapsedCycles, targetCycles);
}