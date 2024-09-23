#include "CPU.hpp"

#include <iostream>

void CPU::reset(Memory& memory)
{
	// Reset registers
	mPc = PC_RESET;
	mSp = SP_RESET;

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

dword CPU::execute(dword cycles, Memory &memory)
{
	dword cyclesRequested = cycles;
	while (cycles > 0)
	{
		// Fetch & Execute an instruction
		byte instruction = fetchByte(cycles, memory);
		switch (instruction)
		{
		case JSR.opcode:
			jsr(cycles, memory);
			break;

		case LDA_IMM.opcode:
			ldaImm(cycles, memory);
			break;

		case LDA_ZP.opcode:
			ldaZp(cycles, memory);
			break;
		
		case LDA_ZPX.opcode:
			ldaZpX(cycles, memory);
			break;
		
		case LDA_ABS.opcode:
			ldaAbs(cycles, memory);
			break;

		case LDA_ABSX.opcode:
			ldaAbsX(cycles, memory);
			break;
		
		case LDA_ABSY.opcode:
			ldaAbsY(cycles, memory);
			break;
		
		case LDA_INDX.opcode:
			ldaIndX(cycles, memory);
			break;
		
		case LDA_INDY.opcode:
			ldaIndY(cycles, memory);
			break;

		default:
			std::cout << "Instruction not handled: " << std::hex << instruction << std::dec << std::endl;
			cycles--;
			break;
		}
	}

	return cyclesRequested - cycles;
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
	cycles--;

	// Get MSB from memory & increment program counter
	data |= (word)memory[mPc] << 8;
	mPc++;
	cycles--;

	return data;
}

byte CPU::readByte(dword &cycles, Memory &memory, dword address)
{
	// Get one byte from memory & decrement cycles counter
	byte data = memory[address];
	cycles--;

	return data;
}

void CPU::stackPush(dword &cycles, Memory &memory, byte value)
{
	// Store value on to the stack pointer's position
	memory[SP_PAGE_OFFSET | mSp] = value;

	// Decrement stack pointer & cycles count
	mSp--;
	cycles--;
}

void CPU::jsr(dword &cycles, Memory &memory)
{
	// Read subroutine address (2 cycles)
	word subroutineAddress = fetchWord(cycles, memory);

	// Push Program counter -1 (return address) to stack (2 cycles)
	word returnAddress = mPc - 1;
	byte returnAddressLsb = (returnAddress & 0x00FF);
	byte returnAddressMsb = (returnAddress & 0xFF00) >> 8;
	stackPush(cycles, memory, returnAddressMsb);
	stackPush(cycles, memory, returnAddressLsb);

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

void CPU::ldaAbs(dword &cycles, Memory &memory)
{
	// Fetch absolute address
	word address = fetchWord(cycles, memory);

	// Read value
	mA = memory[address];
	cycles--;

	// Update status flags
	ldaUpdateStatus();
}

void CPU::ldaAbsX(dword &cycles, Memory &memory)
{
	// Fetch absolute address
	word address = fetchWord(cycles, memory);
	byte prevMsb = (address & 0xFF00) >> 8;

	// Add value of register X -> Wraps to zero page if needed
	address = ((uint32_t)address + mX) & 0x0000FFFFF;
	byte msb = (address & 0xFF00) >> 8;

	// If we go to the next page, add a cycle
	if (msb != prevMsb)
		cycles--;

	// Read value
	mA = memory[address];
	cycles--;

	// Update status flags
	ldaUpdateStatus();
}

void CPU::ldaAbsY(dword &cycles, Memory &memory)
{
	// Fetch absolute address
	word address = fetchWord(cycles, memory);
	byte prevMsb = (address & 0xFF00) >> 8;

	// Add value of register X -> Wraps to zero page if needed
	address = ((uint32_t)address + mY) & 0x0000FFFFF;
	byte msb = (address & 0xFF00) >> 8;

	// If we go to the next page, add a cycle
	if (msb != prevMsb)
		cycles--;

	// Read value
	mA = memory[address];
	cycles--;

	// Update status flags
	ldaUpdateStatus();
}

void CPU::ldaIndX(dword &cycles, Memory &memory)
{
	// Fetch zero page address
	word address = ZP_START + fetchByte(cycles, memory);

	// Add value of X register to the address
	address = (address + mX) & 0x00FF;
	cycles--;

	// Fetch absolute address LSB 
	byte targetAddressLsb = readByte(cycles, memory, address);
	// Fetch absolute address MSB 
	address = (address + 1 ) & 0x00FF;
	byte targetAddressMsb = readByte(cycles, memory, address);

	// Combine LSB & MSB
	word targetAddress = ((word)targetAddressMsb << 8) | targetAddressLsb;

	// Read value
	mA = memory[targetAddress];
	cycles--;

	// Update status flags
	ldaUpdateStatus();
}

void CPU::ldaIndY(dword &cycles, Memory &memory)
{
	// Fetch zero page address
	word zpAddress = ZP_START + fetchByte(cycles, memory);

	// Fetch absolute address LSB 
	byte targetAddressLsb = readByte(cycles, memory, zpAddress);
	// Fetch absolute address MSB 
	zpAddress = (zpAddress + 1 ) & 0x00FF;
	byte targetAddressMsb = readByte(cycles, memory, zpAddress);

	// Combine LSB & MSB
	word targetAddress = ((word)targetAddressMsb << 8) | targetAddressLsb;

	// Add Y to target address
	targetAddress = ((uint32_t)targetAddress + mY) & 0xFFFF;

	// Read value
	mA = memory[targetAddress];
	cycles--;

	// If LSB has a carry for MSB, add one cycle
	if (targetAddressMsb != (targetAddress & 0xFF00) >> 8) 
		cycles--;

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
