#pragma once

// Unnecessary define, but allow better IDE informations
#ifndef TEST
#define TEST
#endif

#include "NES/CPU.hpp"
#include "NES/Memory.hpp"
#include "gtest/gtest.h"


class CPUTests : public testing::Test
{
public:
	void SetUp() override;
	void TearDown() override;

protected:
	CPU cpu;
	Memory memory;
};
