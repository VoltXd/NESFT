#include "CPUTests.hpp"

void CPUTests::SetUp()
{
	cpu.reset(memory);
}

void CPUTests::TearDown()
{
}

TEST_F(CPUTests, ldaImmWorks)
{
	// ********** LDA Immediate address mode test ********** //
	// Positive value
	memory[PC_RESET] = INS_LDA_IMM;
	memory[PC_RESET + 1] = 0x42;
	cpu.execute(2, memory);

	EXPECT_EQ(cpu.getA(), 0x42);
	EXPECT_EQ(cpu.getZ(), 0);
	EXPECT_EQ(cpu.getN(), 0);

	// Negative value
	cpu.reset(memory);
	memory[PC_RESET] = INS_LDA_IMM;
	memory[PC_RESET + 1] = 0xD8;
	cpu.execute(2, memory);

	EXPECT_EQ(cpu.getA(), 0xD8);
	EXPECT_EQ(cpu.getZ(), 0);
	EXPECT_EQ(cpu.getN(), 1);
	
	// Null value
	cpu.reset(memory);
	memory[PC_RESET] = INS_LDA_IMM;
	memory[PC_RESET + 1] = 0x00;
	cpu.execute(2, memory);

	EXPECT_EQ(cpu.getA(), 0x00);
	EXPECT_EQ(cpu.getZ(), 1);
	EXPECT_EQ(cpu.getN(), 0);
}
