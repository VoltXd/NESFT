#include "CPUTests.hpp"

void CPUTests::SetUp()
{
	memory[RESET_VECTOR_LSB] = TEST_MAIN_ADDRESS & 0x00FF;
	memory[RESET_VECTOR_MSB] = (TEST_MAIN_ADDRESS & 0xFF00) >> 8;
	cpu.reset(memory);
}

void CPUTests::TearDown()
{
}
