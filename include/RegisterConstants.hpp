#pragma once

#include "Config.hpp"
#include "Instruction.hpp"

// ********** Memory ********** //
constexpr word ZP_START = 0x0000;

// ********** Register ********** //
constexpr word PC_RESET = 0xFFFC;
constexpr byte SP_RESET = 0xFF;
constexpr word SP_PAGE_OFFSET = 0x0100;

// ********** Instructions (opcode, cycles) ********** //
constexpr instruction JSR  	   = { 0x20, 6 };
constexpr instruction LDA_IMM  = { 0xA9, 2 };
constexpr instruction LDA_ZP   = { 0xA5, 3 };
constexpr instruction LDA_ZPX  = { 0xB5, 4 };
constexpr instruction LDA_ABS  = { 0xAD, 4 };
constexpr instruction LDA_ABSX = { 0xBD, 4 };
constexpr instruction LDA_ABSY = { 0xB9, 4 };
constexpr instruction LDA_INDX = { 0xA1, 6 };
constexpr instruction LDA_INDY = { 0xB1, 5 };
