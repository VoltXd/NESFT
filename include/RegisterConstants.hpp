#pragma once

#include "Config.hpp"

// ********** Memory ********** //
constexpr word ZP_START = 0x0000;
constexpr word PC_RESET = 0xFFFC;

// ********** Instructions ********** //
constexpr byte INS_JSR = 0x20;
constexpr byte INS_LDA_IMM = 0xA9;
constexpr byte INS_LDA_ZP = 0xA5;
constexpr byte INS_LDA_ZPX = 0xB5;
