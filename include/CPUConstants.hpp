#pragma once

#include "Config.hpp"
#include "Instruction.hpp"

// ********** Register ********** //
constexpr word PC_RESET = 0xFFFC;
constexpr byte SP_RESET = 0xFF;
constexpr word SP_PAGE_OFFSET = 0x0100;

// ********** Instructions (operation, address mode, cycles, opcode) ********** //
constexpr instruction_t ADC_IMM   = { Operation::ADC, AddressingMode::Immediate,   2, 0x69 };
constexpr instruction_t ADC_ZP    = { Operation::ADC, AddressingMode::ZeroPage,    3, 0x65 };
constexpr instruction_t ADC_ZPX   = { Operation::ADC, AddressingMode::ZeroPageX,   4, 0x75 };
constexpr instruction_t ADC_ABS   = { Operation::ADC, AddressingMode::Absolute,    4, 0x6D };
constexpr instruction_t ADC_ABSX  = { Operation::ADC, AddressingMode::AbsoluteX,   4, 0x7D };
constexpr instruction_t ADC_ABSY  = { Operation::ADC, AddressingMode::AbsoluteY,   4, 0x79 };
constexpr instruction_t ADC_INDX  = { Operation::ADC, AddressingMode::IndirectX,   6, 0x61 };
constexpr instruction_t ADC_INDY  = { Operation::ADC, AddressingMode::IndirectY,   5, 0x71 };

constexpr instruction_t AND_IMM   = { Operation::AND, AddressingMode::Immediate,   2, 0x29 };
constexpr instruction_t AND_ZP    = { Operation::AND, AddressingMode::ZeroPage,    3, 0x25 };
constexpr instruction_t AND_ZPX   = { Operation::AND, AddressingMode::ZeroPageX,   4, 0x35 };
constexpr instruction_t AND_ABS   = { Operation::AND, AddressingMode::Absolute,    4, 0x2D };
constexpr instruction_t AND_ABSX  = { Operation::AND, AddressingMode::AbsoluteX,   4, 0x3D };
constexpr instruction_t AND_ABSY  = { Operation::AND, AddressingMode::AbsoluteY,   4, 0x39 };
constexpr instruction_t AND_INDX  = { Operation::AND, AddressingMode::IndirectX,   6, 0x21 };
constexpr instruction_t AND_INDY  = { Operation::AND, AddressingMode::IndirectY,   5, 0x31 };

constexpr instruction_t ASL_ACC   = { Operation::ASL, AddressingMode::Accumulator, 1, 0x0A };
constexpr instruction_t ASL_ZP    = { Operation::ASL, AddressingMode::ZeroPage,    2, 0x06 };
constexpr instruction_t ASL_ZPX   = { Operation::ASL, AddressingMode::ZeroPageX,   2, 0x16 };
constexpr instruction_t ASL_ABS   = { Operation::ASL, AddressingMode::Absolute,    3, 0x0E };
constexpr instruction_t ASL_ABSX  = { Operation::ASL, AddressingMode::AbsoluteX,   3, 0x1E };

constexpr instruction_t BCC  	    = { Operation::BCC, AddressingMode::Relative,    2, 0x90 };

constexpr instruction_t BCS  	    = { Operation::BCS, AddressingMode::Relative,    2, 0xB0 };

constexpr instruction_t BEQ  	    = { Operation::BEQ, AddressingMode::Relative,    2, 0xF0 };

constexpr instruction_t BIT_ZP	= { Operation::BIT, AddressingMode::ZeroPage,    3, 0x24 };
constexpr instruction_t BIT_ABS	= { Operation::BIT, AddressingMode::Absolute,    4, 0x2C };

constexpr instruction_t BMI  	    = { Operation::BMI, AddressingMode::Relative,    2, 0x30 };

constexpr instruction_t BNE  	    = { Operation::BNE, AddressingMode::Relative,    2, 0xD0 };

constexpr instruction_t BPL  	    = { Operation::BPL, AddressingMode::Relative,    2, 0x10 };

constexpr instruction_t BRK  	    = { Operation::BRK, AddressingMode::Implicit,    7, 0x00 };

constexpr instruction_t BVC  	    = { Operation::BVC, AddressingMode::Relative,    2, 0x50 };

constexpr instruction_t BVS  	    = { Operation::BVS, AddressingMode::Relative,    2, 0x70 };

constexpr instruction_t CLC  	    = { Operation::CLC, AddressingMode::Implicit,    2, 0x18 };

constexpr instruction_t CLD  	    = { Operation::CLD, AddressingMode::Implicit,    2, 0xD8 };

constexpr instruction_t CLI  	    = { Operation::CLI, AddressingMode::Implicit,    2, 0x58 };

constexpr instruction_t CLV  	    = { Operation::CLV, AddressingMode::Implicit,    2, 0xB8 };

constexpr instruction_t CMP_IMM   = { Operation::CMP, AddressingMode::Immediate,   2, 0xC9 };
constexpr instruction_t CMP_ZP    = { Operation::CMP, AddressingMode::ZeroPage,    3, 0xC5 };
constexpr instruction_t CMP_ZPX   = { Operation::CMP, AddressingMode::ZeroPageX,   4, 0xD5 };
constexpr instruction_t CMP_ABS   = { Operation::CMP, AddressingMode::Absolute,    4, 0xCD };
constexpr instruction_t CMP_ABSX  = { Operation::CMP, AddressingMode::AbsoluteX,   4, 0xDD };
constexpr instruction_t CMP_ABSY  = { Operation::CMP, AddressingMode::AbsoluteY,   4, 0xD9 };
constexpr instruction_t CMP_INDX  = { Operation::CMP, AddressingMode::IndirectX,   6, 0xC1 };
constexpr instruction_t CMP_INDY  = { Operation::CMP, AddressingMode::IndirectY,   5, 0xD1 };

constexpr instruction_t CPX_IMM   = { Operation::CPX, AddressingMode::Immediate,   2, 0xE0 };
constexpr instruction_t CPX_ZP    = { Operation::CPX, AddressingMode::ZeroPage,    3, 0xE4 };
constexpr instruction_t CPX_ABS   = { Operation::CPX, AddressingMode::Absolute,    4, 0xEC };

constexpr instruction_t CPY_IMM   = { Operation::CPY, AddressingMode::Immediate,   2, 0xC0 };
constexpr instruction_t CPY_ZP    = { Operation::CPY, AddressingMode::ZeroPage,    3, 0xC4 };
constexpr instruction_t CPY_ABS   = { Operation::CPY, AddressingMode::Absolute,    4, 0xCC };

constexpr instruction_t DEC_ZP    = { Operation::DEC, AddressingMode::ZeroPage,    5, 0xC6 };
constexpr instruction_t DEC_ZPX   = { Operation::DEC, AddressingMode::ZeroPageX,   6, 0xD6 };
constexpr instruction_t DEC_ABS   = { Operation::DEC, AddressingMode::Absolute,    6, 0xCE };
constexpr instruction_t DEC_ABSX  = { Operation::DEC, AddressingMode::AbsoluteX,   7, 0xDE };

constexpr instruction_t DEX       = { Operation::DEX, AddressingMode::Implicit,    2, 0xCA };

constexpr instruction_t DEY       = { Operation::DEY, AddressingMode::Implicit,    2, 0x88 };

constexpr instruction_t EOR_IMM   = { Operation::EOR, AddressingMode::Immediate,   2, 0x49 };
constexpr instruction_t EOR_ZP    = { Operation::EOR, AddressingMode::ZeroPage,    3, 0x45 };
constexpr instruction_t EOR_ZPX   = { Operation::EOR, AddressingMode::ZeroPageX,   4, 0x55 };
constexpr instruction_t EOR_ABS   = { Operation::EOR, AddressingMode::Absolute,    4, 0x4D };
constexpr instruction_t EOR_ABSX  = { Operation::EOR, AddressingMode::AbsoluteX,   4, 0x5D };
constexpr instruction_t EOR_ABSY  = { Operation::EOR, AddressingMode::AbsoluteY,   4, 0x59 };
constexpr instruction_t EOR_INDX  = { Operation::EOR, AddressingMode::IndirectX,   6, 0x41 };
constexpr instruction_t EOR_INDY  = { Operation::EOR, AddressingMode::IndirectY,   5, 0x51 };

constexpr instruction_t INC_ZP    = { Operation::INC, AddressingMode::ZeroPage,    5, 0xE6 };
constexpr instruction_t INC_ZPX   = { Operation::INC, AddressingMode::ZeroPageX,   6, 0xF6 };
constexpr instruction_t INC_ABS   = { Operation::INC, AddressingMode::Absolute,    6, 0xEE };
constexpr instruction_t INC_ABSX  = { Operation::INC, AddressingMode::AbsoluteX,   7, 0xFE };

constexpr instruction_t INX       = { Operation::INX, AddressingMode::Implicit,    2, 0xE8 };

constexpr instruction_t INY       = { Operation::INY, AddressingMode::Implicit,    2, 0xC8 };

constexpr instruction_t JMP_ABS  	= { Operation::JMP, AddressingMode::Absolute ,   3, 0x4C };
constexpr instruction_t JMP_IND  	= { Operation::JMP, AddressingMode::Indirect ,   5, 0x6C };

constexpr instruction_t JSR  	    = { Operation::JSR, AddressingMode::Absolute ,   6, 0x20 };

constexpr instruction_t LDA_IMM   = { Operation::LDA, AddressingMode::Immediate,   2, 0xA9 };
constexpr instruction_t LDA_ZP    = { Operation::LDA, AddressingMode::ZeroPage,    3, 0xA5 };
constexpr instruction_t LDA_ZPX   = { Operation::LDA, AddressingMode::ZeroPageX,   4, 0xB5 };
constexpr instruction_t LDA_ABS   = { Operation::LDA, AddressingMode::Absolute,    4, 0xAD };
constexpr instruction_t LDA_ABSX  = { Operation::LDA, AddressingMode::AbsoluteX,   4, 0xBD };
constexpr instruction_t LDA_ABSY  = { Operation::LDA, AddressingMode::AbsoluteY,   4, 0xB9 };
constexpr instruction_t LDA_INDX  = { Operation::LDA, AddressingMode::IndirectX,   6, 0xA1 };
constexpr instruction_t LDA_INDY  = { Operation::LDA, AddressingMode::IndirectY,   5, 0xB1 };

constexpr instruction_t LDX_IMM   = { Operation::LDX, AddressingMode::Immediate,   2, 0xA2 };
constexpr instruction_t LDX_ZP    = { Operation::LDX, AddressingMode::ZeroPage,    3, 0xA6 };
constexpr instruction_t LDX_ZPY   = { Operation::LDX, AddressingMode::ZeroPageY,   4, 0xB6 };
constexpr instruction_t LDX_ABS   = { Operation::LDX, AddressingMode::Absolute,    4, 0xAE };
constexpr instruction_t LDX_ABSY  = { Operation::LDX, AddressingMode::AbsoluteY,   4, 0xBE };

constexpr instruction_t LDY_IMM   = { Operation::LDY, AddressingMode::Immediate,   2, 0xA0 };
constexpr instruction_t LDY_ZP    = { Operation::LDY, AddressingMode::ZeroPage,    3, 0xA4 };
constexpr instruction_t LDY_ZPX   = { Operation::LDY, AddressingMode::ZeroPageX,   4, 0xB4 };
constexpr instruction_t LDY_ABS   = { Operation::LDY, AddressingMode::Absolute,    4, 0xAC };
constexpr instruction_t LDY_ABSX  = { Operation::LDY, AddressingMode::AbsoluteX,   4, 0xBC };

constexpr instruction_t LSR_ACC   = { Operation::LSR, AddressingMode::Accumulator, 2, 0x4A };
constexpr instruction_t LSR_ZP    = { Operation::LSR, AddressingMode::ZeroPage,    5, 0x46 };
constexpr instruction_t LSR_ZPX   = { Operation::LSR, AddressingMode::ZeroPageX,   6, 0x56 };
constexpr instruction_t LSR_ABS   = { Operation::LSR, AddressingMode::Absolute,    6, 0x4E };
constexpr instruction_t LSR_ABSX  = { Operation::LSR, AddressingMode::AbsoluteX,   7, 0x5E };

constexpr instruction_t NOP       = { Operation::NOP, AddressingMode::Implicit,    2, 0xEA };

constexpr instruction_t ORA_IMM   = { Operation::ORA, AddressingMode::Immediate,   2, 0x09 };
constexpr instruction_t ORA_ZP    = { Operation::ORA, AddressingMode::ZeroPage,    3, 0x05 };
constexpr instruction_t ORA_ZPX   = { Operation::ORA, AddressingMode::ZeroPageX,   4, 0x15 };
constexpr instruction_t ORA_ABS   = { Operation::ORA, AddressingMode::Absolute,    4, 0x0D };
constexpr instruction_t ORA_ABSX  = { Operation::ORA, AddressingMode::AbsoluteX,   4, 0x1D };
constexpr instruction_t ORA_ABSY  = { Operation::ORA, AddressingMode::AbsoluteY,   4, 0x19 };
constexpr instruction_t ORA_INDX  = { Operation::ORA, AddressingMode::IndirectX,   6, 0x01 };
constexpr instruction_t ORA_INDY  = { Operation::ORA, AddressingMode::IndirectY,   5, 0x11 };

constexpr instruction_t PHA       = { Operation::PHA, AddressingMode::Implicit,    3, 0x48 };

constexpr instruction_t PHP       = { Operation::PHP, AddressingMode::Implicit,    3, 0x08 };

constexpr instruction_t PLA       = { Operation::PLA, AddressingMode::Implicit,    4, 0x68 };

constexpr instruction_t PLP       = { Operation::PLP, AddressingMode::Implicit,    4, 0x28 };

constexpr instruction_t ROL_ACC   = { Operation::ROL, AddressingMode::Accumulator, 2, 0x2A };
constexpr instruction_t ROL_ZP    = { Operation::ROL, AddressingMode::ZeroPage,    5, 0x26 };
constexpr instruction_t ROL_ZPX   = { Operation::ROL, AddressingMode::ZeroPageX,   6, 0x36 };
constexpr instruction_t ROL_ABS   = { Operation::ROL, AddressingMode::Absolute,    6, 0x2E };
constexpr instruction_t ROL_ABSX  = { Operation::ROL, AddressingMode::AbsoluteX,   7, 0x3E };

constexpr instruction_t ROR_ACC   = { Operation::ROR, AddressingMode::Accumulator, 2, 0x6A };
constexpr instruction_t ROR_ZP    = { Operation::ROR, AddressingMode::ZeroPage,    5, 0x66 };
constexpr instruction_t ROR_ZPX   = { Operation::ROR, AddressingMode::ZeroPageX,   6, 0x76 };
constexpr instruction_t ROR_ABS   = { Operation::ROR, AddressingMode::Absolute,    6, 0x6E };
constexpr instruction_t ROR_ABSX  = { Operation::ROR, AddressingMode::AbsoluteX,   7, 0x7E };

constexpr instruction_t RTI 	    = { Operation::RTI, AddressingMode::Implicit,    6, 0x40 };

constexpr instruction_t RTS 	    = { Operation::RTS, AddressingMode::Implicit,    6, 0x60 };

constexpr instruction_t SBC_IMM   = { Operation::SBC, AddressingMode::Immediate,   2, 0xE9 };
constexpr instruction_t SBC_ZP    = { Operation::SBC, AddressingMode::ZeroPage,    3, 0xE5 };
constexpr instruction_t SBC_ZPX   = { Operation::SBC, AddressingMode::ZeroPageX,   4, 0xF5 };
constexpr instruction_t SBC_ABS   = { Operation::SBC, AddressingMode::Absolute,    4, 0xED };
constexpr instruction_t SBC_ABSX  = { Operation::SBC, AddressingMode::AbsoluteX,   4, 0xFD };
constexpr instruction_t SBC_ABSY  = { Operation::SBC, AddressingMode::AbsoluteY,   4, 0xF9 };
constexpr instruction_t SBC_INDX  = { Operation::SBC, AddressingMode::IndirectX,   6, 0xE1 };
constexpr instruction_t SBC_INDY  = { Operation::SBC, AddressingMode::IndirectY,   5, 0xF1 };

constexpr instruction_t SEC 	    = { Operation::SEC, AddressingMode::Implicit,    2, 0x38 };

constexpr instruction_t SED 	    = { Operation::SED, AddressingMode::Implicit,    2, 0xF8 };

constexpr instruction_t SEI 	    = { Operation::SEI, AddressingMode::Implicit,    2, 0x78 };

constexpr instruction_t STA_ZP    = { Operation::STA, AddressingMode::ZeroPage,    3, 0x85 };
constexpr instruction_t STA_ZPX   = { Operation::STA, AddressingMode::ZeroPageX,   4, 0x95 };
constexpr instruction_t STA_ABS   = { Operation::STA, AddressingMode::Absolute,    4, 0x8D };
constexpr instruction_t STA_ABSX  = { Operation::STA, AddressingMode::AbsoluteX,   5, 0x9D };
constexpr instruction_t STA_ABSY  = { Operation::STA, AddressingMode::AbsoluteY,   5, 0x99 };
constexpr instruction_t STA_INDX  = { Operation::STA, AddressingMode::IndirectX,   6, 0x81 };
constexpr instruction_t STA_INDY  = { Operation::STA, AddressingMode::IndirectY,   6, 0x91 };

constexpr instruction_t STX_ZP    = { Operation::STX, AddressingMode::ZeroPage,    3, 0x86 };
constexpr instruction_t STX_ZPY   = { Operation::STX, AddressingMode::ZeroPageY,   4, 0x96 };
constexpr instruction_t STX_ABS   = { Operation::STX, AddressingMode::Absolute,    4, 0x8E };

constexpr instruction_t STY_ZP    = { Operation::STY, AddressingMode::ZeroPage,    3, 0x84 };
constexpr instruction_t STY_ZPX   = { Operation::STY, AddressingMode::ZeroPageX,   4, 0x94 };
constexpr instruction_t STY_ABS   = { Operation::STY, AddressingMode::Absolute,    4, 0x8C };

constexpr instruction_t TAX 	    = { Operation::TAX, AddressingMode::Implicit,    2, 0xAA };

constexpr instruction_t TAY 	    = { Operation::TAY, AddressingMode::Implicit,    2, 0xA8 };

constexpr instruction_t TSX 	    = { Operation::TSX, AddressingMode::Implicit,    2, 0xBA };

constexpr instruction_t TXA 	    = { Operation::TXA, AddressingMode::Implicit,    2, 0x8A };

constexpr instruction_t TXS 	    = { Operation::TXS, AddressingMode::Implicit,    2, 0x9A };

constexpr instruction_t TYA 	    = { Operation::TYA, AddressingMode::Implicit,    2, 0x98 };

constexpr instruction_t XXX 	    = { Operation::XXX, AddressingMode::Implicit,    7, 0xFF }; // Used to kill the program on illegal instruction

constexpr instruction_t INSTRUCTION_LUT[] = 
{
	BRK,      // 0x00
	ORA_INDX, // 0x01
	XXX,      // 0x02
	XXX,      // 0x03
	XXX,      // 0x04
	ORA_ZP,   // 0x05
	ASL_ZP,   // 0x06
	XXX,      // 0x07
	PHP,      // 0x08
	ORA_IMM,  // 0x09
	ASL_ACC,  // 0x0A
	XXX,      // 0x0B
	XXX,      // 0x0C
	ORA_ABS,  // 0x0D
	ASL_ABS,  // 0x0E
	XXX,      // 0x0F
	BPL,      // 0x10
	ORA_INDY, // 0x11
	XXX,      // 0x12
	XXX,      // 0x13
	XXX,      // 0x14
	ORA_ZPX,  // 0x15
	ASL_ZPX,  // 0x16
	XXX,      // 0x17
	CLC,      // 0x18
	ORA_ABSY, // 0x19
	XXX,      // 0x1A
	XXX,      // 0x1B
	XXX,      // 0x1C
	ORA_ABSX, // 0x1D
	ASL_ABSX, // 0x1E
	XXX,      // 0x1F
	JSR,      // 0x20
	AND_INDX, // 0x21
	XXX,      // 0x22
	XXX,      // 0x23
	BIT_ZP,   // 0x24
	AND_ZP,   // 0x25
	ROL_ZP,   // 0x26
	XXX,      // 0x27
	PLP,      // 0x28
	AND_IMM,  // 0x29
	ROL_ACC,  // 0x2A
	XXX,      // 0x2B
	BIT_ABS,  // 0x2C
	AND_ABS,  // 0x2D
	ROL_ABS,  // 0x2E
	XXX,      // 0x2F
	BMI,      // 0x30
	AND_INDY, // 0x31
	XXX,      // 0x32
	XXX,      // 0x33
	XXX,      // 0x34
	AND_ZPX,  // 0x35
	ROL_ZPX,  // 0x36
	XXX,      // 0x37
	SEC,      // 0x38
	AND_ABSY, // 0x39
	XXX,      // 0x3A
	XXX,      // 0x3B
	XXX,      // 0x3C
	AND_ABSX, // 0x3D
	ROL_ABSX, // 0x3E
	XXX,      // 0x3F
	RTI,      // 0x40
	EOR_INDX, // 0x41
	XXX,      // 0x42
	XXX,      // 0x43
	XXX,      // 0x44
	EOR_ZP,   // 0x45
	LSR_ZP,   // 0x46
	XXX,      // 0x47
	PHA,      // 0x48
	EOR_IMM,  // 0x49
	LSR_ACC,  // 0x4A
	XXX,      // 0x4B
	JMP_ABS,  // 0x4C
	EOR_ABS,  // 0x4D
	LSR_ABS,  // 0x4E
	XXX,      // 0x4F
	BVC,      // 0x50
	EOR_INDY, // 0x51
	XXX,      // 0x52
	XXX,      // 0x53
	XXX,      // 0x54
	EOR_ZPX,  // 0x55
	LSR_ZPX,  // 0x56
	XXX,      // 0x57
	CLI,      // 0x58
	EOR_ABSY, // 0x59
	XXX,      // 0x5A
	XXX,      // 0x5B
	XXX,      // 0x5C
	EOR_ABSX, // 0x5D
	LSR_ABSX, // 0x5E
	XXX,      // 0x5F
	RTS,      // 0x60
	ADC_INDX, // 0x61
	XXX,      // 0x62
	XXX,      // 0x63
	XXX,      // 0x64
	ADC_ZP,   // 0x65
	ROR_ZP,   // 0x66
	XXX,      // 0x67
	PLA,      // 0x68
	ADC_IMM,  // 0x69
	ROR_ACC,  // 0x6A
	XXX,      // 0x6B
	JMP_IND,  // 0x6C
	ADC_ABS,  // 0x6D
	ROR_ABS,  // 0x6E
	XXX,      // 0x6F
	BVS,      // 0x70
	ADC_INDY, // 0x71
	XXX,      // 0x72
	XXX,      // 0x73
	XXX,      // 0x74
	ADC_ZPX,  // 0x75
	ROR_ZPX,  // 0x76
	XXX,      // 0x77
	SEI,      // 0x78
	ADC_ABSY, // 0x79
	XXX,      // 0x7A
	XXX,      // 0x7B
	XXX,      // 0x7C
	ADC_ABSX, // 0x7D
	ROR_ABSX, // 0x7E
	XXX,      // 0x7F
	XXX,      // 0x80
	STA_INDX, // 0x81
	XXX,      // 0x82
	XXX,      // 0x83
	STY_ZP,   // 0x84
	STA_ZP,   // 0x85
	STX_ZP,   // 0x86
	XXX,      // 0x87
	DEY,      // 0x88
	XXX,      // 0x89
	TXA,      // 0x8A
	XXX,      // 0x8B
	STY_ABS,  // 0x8C
	STA_ABS,  // 0x8D
	STX_ABS,  // 0x8E
	XXX,      // 0x8F
	BCC,      // 0x90
	STA_INDY, // 0x91
	XXX,      // 0x92
	XXX,      // 0x93
	STY_ZPX,  // 0x94
	STA_ZPX,  // 0x95
	STX_ZPY,  // 0x96
	XXX,      // 0x97
	TYA,      // 0x98
	STA_ABSY, // 0x99
	TXS,      // 0x9A
	XXX,      // 0x9B
	XXX,      // 0x9C
	STA_ABSX, // 0x9D
	XXX,      // 0x9E
	XXX,      // 0x9F
	LDY_IMM,  // 0xA0
	LDA_INDX, // 0xA1
	LDX_IMM,  // 0xA2
	XXX,      // 0xA3
	LDY_ZP,   // 0xA4
	LDA_ZP,   // 0xA5
	LDX_ZP,   // 0xA6
	XXX,      // 0xA7
	TAY,      // 0xA8
	LDA_IMM,  // 0xA9
	TAX,      // 0xAA
	XXX,      // 0xAB
	LDY_ABS,  // 0xAC
	LDA_ABS,  // 0xAD
	LDX_ABS,  // 0xAE
	XXX,      // 0xAF
	BCS,      // 0xB0
	LDA_INDY, // 0xB1
	XXX,      // 0xB2
	XXX,      // 0xB3
	LDY_ZPX,  // 0xB4
	LDA_ZPX,  // 0xB5
	LDX_ZPY,  // 0xB6
	XXX,      // 0xB7
	CLV,      // 0xB8
	LDA_ABSY, // 0xB9
	TSX,      // 0xBA
	XXX,      // 0xBB
	LDY_ABSX, // 0xBC
	LDA_ABSX, // 0xBD
	LDX_ABSY, // 0xBE
	XXX,      // 0xBF
	CPY_IMM,  // 0xC0
	CMP_INDX, // 0xC1
	XXX,      // 0xC2
	XXX,      // 0xC3
	CPY_ZP,   // 0xC4
	CMP_ZP,   // 0xC5
	DEC_ZP,   // 0xC6
	XXX,      // 0xC7
	INY,      // 0xC8
	CMP_IMM,  // 0xC9
	DEX,      // 0xCA
	XXX,      // 0xCB
	CPY_ABS,  // 0xCC
	CMP_ABS,  // 0xCD
	DEC_ABS,  // 0xCE
	XXX,      // 0xCF
	BNE,      // 0xD0
	CMP_INDY, // 0xD1
	XXX,      // 0xD2
	XXX,      // 0xD3
	XXX,      // 0xD4
	CMP_ZPX,  // 0xD5
	DEC_ZPX,  // 0xD6
	XXX,      // 0xD7
	CLD,      // 0xD8
	CMP_ABSY, // 0xD9
	XXX,      // 0xDA
	XXX,      // 0xDB
	XXX,      // 0xDC
	CMP_ABSX, // 0xDD
	DEC_ABSX, // 0xDE
	XXX,      // 0xDF
	CPX_IMM,  // 0xE0
	SBC_INDX, // 0xE1
	XXX,      // 0xE2
	XXX,      // 0xE3
	CPX_ZP,   // 0xE4
	SBC_ZP,   // 0xE5
	INC_ZP,   // 0xE6
	XXX,      // 0xE7
	INX,      // 0xE8
	SBC_IMM,  // 0xE9
	NOP,      // 0xEA
	XXX,      // 0xEB
	CPX_ABS,  // 0xEC
	SBC_ABS,  // 0xED
	INC_ABS,  // 0xEE
	XXX,      // 0xEF
	BEQ,      // 0xF0
	SBC_INDY, // 0xF1
	XXX,      // 0xF2
	XXX,      // 0xF3
	XXX,      // 0xF4
	SBC_ZPX,  // 0xF5
	INC_ZPX,  // 0xF6
	XXX,      // 0xF7
	SED,      // 0xF8
	SBC_ABSY, // 0xF9
	XXX,      // 0xFA
	XXX,      // 0xFB
	XXX,      // 0xFC
	SBC_ABSX, // 0xFD
	INC_ABSX, // 0xFE
	XXX       // 0xFF
};
