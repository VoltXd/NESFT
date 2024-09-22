#include "CPU.hpp"

#include <iostream>

void CPU::reset(Memory& memory)
{
	// Reset registers
	mPc = PC_RESET;
	mSp = 0xFF;

	mA = 0;
	mX = 0;
	mY = 0;

	mC = 0;
	mZ = 0;
	mI = 0;
	mD = 0;
	mB = 0;
	mV = 0;
	mN = 0;

	// Reset memory
	memory.initialise();
}

void CPU::execute(dword cycles, Memory &memory)
{
	while (cycles > 0)
	{
		// Fetch & Execute an instruction
		byte instruction = fetchByte(cycles, memory);
		switch (instruction)
		{
		case INS_JSR:
			jsr(cycles, memory);
			break;

		case INS_LDA_IMM:
			ldaImm(cycles, memory);
			break;

		case INS_LDA_ZP:
			ldaZp(cycles, memory);
			break;
		
		case INS_LDA_ZPX:
			ldaZpX(cycles, memory);
			break;

		default:
			std::cout << "Instruction not handled: " << std::hex << instruction << std::dec << std::endl;
			break;
		}
	}
}

byte CPU::fetchByte(dword& cycles, Memory &memory)
{
	// Get one byte from memory & increment program counter
	byte data = memory[mPc];
	mPc++;

	// Decrement remaining cycles counter
	cycles--;

	return data;
}

word CPU::fetchWord(dword &cycles, Memory &memory)
{
	// 6502 => Little endian => 1st is LSB, 2nd is MSB
	// Get LSB from memory & increment program counter
	word data = memory[mPc];
	mPc++;

	// Get MSB from memory & increment program counter
	data |= (word)memory[mPc] << 8;
	mPc++;

	// Decrement remaining cycles counter
	cycles -= 2;

	return data;
}

byte CPU::readByte(dword &cycles, Memory &memory, dword address)
{
	// Get one byte from memory & decrement cycles counter
	byte data = memory[address];
	cycles--;

	return data;
}

void CPU::jsr(dword &cycles, Memory &memory)
{
	// Read subroutine address (2 cycles)
	word subroutineAddress = fetchWord(cycles, memory);

	// Push Program counter to stack (2 cycles)
	memory.writeWord(mPc - 1, mSp - 1, cycles);
	mSp -= 2;

	// Set Program counter (1 cycle)
	mPc = subroutineAddress;
	cycles--;
}

void CPU::ldaImm(dword &cycles, Memory &memory)
{
	// Fetch immediate
	byte immediate = fetchByte(cycles, memory);
	mA = immediate;

	// Update status flags
	ldaUpdateStatus();
}

void CPU::ldaZp(dword &cycles, Memory &memory)
{
	// Fetch zero page address 
	word address = ZP_START + fetchByte(cycles, memory);

	// Read memory
	byte data = readByte(cycles, memory, address);
	mA = data;

	// Update status flags
	ldaUpdateStatus();
}

void CPU::ldaZpX(dword &cycles, Memory &memory)
{
	// Fetch zero page address 
	word address = ZP_START + fetchByte(cycles, memory);
	
	// Add the value of register X to address, wraps in case of overflow
	address = (address + mX) & 0x00FF;
	cycles--;	

	// Read memory
	byte data = readByte(cycles, memory, address);
	mA = data;

	// Update status flags
	ldaUpdateStatus();
}

void CPU::ldaUpdateStatus()
{
	// Only Z & N flags need to be updated
	if (mA == 0)
		mZ = 1;

	if (mA & 0b1000'0000)
		mN = 1;
}
