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

sdword CPU::execute(sdword cycles, Memory &memory)
{
	sdword cyclesRequested = cycles;
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

byte CPU::fetchByte(sdword& cycles, Memory &memory)
{
	// Get one byte from memory & increment program counter
	byte data = memory[mPc];
	mPc++;

	// Decrement remaining cycles counter
	cycles--;

	return data;
}

word CPU::fetchWord(sdword &cycles, Memory &memory)
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

byte CPU::readByte(sdword &cycles, Memory &memory, word address)
{
	// Get one byte from memory & decrement cycles counter
	byte data = memory[address];
	cycles--;

	return data;
}

void CPU::stackPush(sdword &cycles, Memory &memory, byte value)
{
	// Store value on to the stack pointer's position
	memory[SP_PAGE_OFFSET | mSp] = value;

	// Decrement stack pointer & cycles count
	mSp--;
	cycles--;
}

byte CPU::fetchImmediate(sdword &cycles, Memory &memory)
{
	// Get the byte following the instruction 
	byte immediate = fetchByte(cycles, memory);
	return immediate;
}

word CPU::fetchAddrZeroPage(sdword &cycles, Memory &memory)
{
	// Fetch zero page address 
	word zpAddress = fetchByte(cycles, memory);
	return zpAddress;
}

word CPU::fetchAddrZeroPageX(sdword &cycles, Memory &memory)
{
	// Fetch zero page address 
	word address = fetchByte(cycles, memory);
	
	// Add the value of register X to address, wraps in case of overflow
	address = (address + mX) & 0x00FF;
	cycles--;

	return address;
}

word CPU::fetchAddrZeroPageY(sdword &cycles, Memory &memory)
{
	// Fetch zero page address 
	word address = fetchByte(cycles, memory);
	
	// Add the value of register Y to address, wraps in case of overflow
	address = (address + mY) & 0x00FF;
	cycles--;

	return address;
	return word();
}

word CPU::fetchAddrRelative(sdword &cycles, Memory &memory)
{
	// Fetch address offset
	// WARNING: the offset is signed, byte is a 8 bits unsigned data type
	byte offset = fetchByte(cycles, memory);
	sbyte signedOffset = *(sbyte*)&offset; 		// Pointer magic ;)

	// Calculate relative address
	word relAddress = mPc + signedOffset;
	return relAddress;
}

word CPU::fetchAddrAbsolute(sdword &cycles, Memory &memory)
{
	// Fetch absolute address
	word address = fetchWord(cycles, memory);
	return address;
}

word CPU::fetchAddrAbsoluteX(sdword &cycles, Memory &memory, bool& hasPageCrossed)
{
	// Fetch absolute address
	word address = fetchWord(cycles, memory);
	byte prevMsb = (address & 0xFF00) >> 8;

	// Add value of register X -> Wraps to zero page if needed
	address = ((uint32_t)address + mX) & 0x0000FFFFF;
	byte msb = (address & 0xFF00) >> 8;

	// If we go to the next page, some instruction will add one more cycle
	hasPageCrossed = msb != prevMsb;

	return address;
}

word CPU::fetchAddrAbsoluteY(sdword &cycles, Memory &memory, bool& hasPageCrossed)
{
	// Fetch absolute address
	word address = fetchWord(cycles, memory);
	byte prevMsb = (address & 0xFF00) >> 8;

	// Add value of register Y -> Wraps to zero page if needed
	address = ((uint32_t)address + mY) & 0x0000FFFFF;
	byte msb = (address & 0xFF00) >> 8;

	// If we go to the next page, some instruction will add one more cycle
	hasPageCrossed = msb != prevMsb;

	return address;
}

word CPU::fetchAddrIndirect(sdword &cycles, Memory &memory)
{
	// Fetch indirect address
	word indirectAddress = fetchWord(cycles, memory);
	byte indirectAddressLsb = indirectAddress & 0x00FF;
	byte indirectAddressMsb = (indirectAddress & 0xFF00) >> 8;

	// Fetch target address
	// WARNING: according to https://www.nesdev.org/obelisk-6502-guide/reference.html#JMP,
	// if indirect address points to a page boundary, the LSB will wrap ($FF -> $00) but
	// not the MSB.
	word indirectAddressToMsb = ((word)indirectAddressMsb << 8) | ((indirectAddressLsb + 1) & 0x00FF); 
	byte targetAddressLsb = readByte(cycles, memory, indirectAddress);
	byte targetAddressMsb = readByte(cycles, memory, indirectAddressToMsb);
	word targetAddress = ((word)targetAddressMsb << 8) | targetAddressLsb;

	return targetAddress;
}

word CPU::fetchAddrIndirectX(sdword &cycles, Memory &memory)
{
	// Fetch zero page address
	word address = fetchByte(cycles, memory);

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

	return targetAddress;
}

word CPU::fetchAddrIndirectY(sdword &cycles, Memory &memory, bool& hasPageCrossed)
{
	// Fetch zero page address
	word zpAddress = fetchByte(cycles, memory);

	// Fetch absolute address LSB 
	byte targetAddressLsb = readByte(cycles, memory, zpAddress);
	// Fetch absolute address MSB 
	zpAddress = (zpAddress + 1 ) & 0x00FF;
	byte targetAddressMsb = readByte(cycles, memory, zpAddress);

	// Combine LSB & MSB
	word targetAddress = ((word)targetAddressMsb << 8) | targetAddressLsb;

	// Add Y to target address
	targetAddress = ((uint32_t)targetAddress + mY) & 0xFFFF;
	
	// If we go to the next page, some instruction will add one more cycle
	hasPageCrossed = targetAddressMsb != (targetAddress & 0xFF00) >> 8;

	return targetAddress;
}

void CPU::jsr(sdword &cycles, Memory &memory)
{
	// Read subroutine address (2 cycles)
	word subroutineAddress = fetchAddrAbsolute(cycles, memory);

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

void CPU::ldaImm(sdword &cycles, Memory &memory)
{
	// Fetch immediate
	mA = fetchImmediate(cycles, memory);

	// Update status flags
	ldaUpdateStatus();
}

void CPU::ldaZp(sdword &cycles, Memory &memory)
{
	// Fetch zero page address 
	word address = fetchAddrZeroPage(cycles, memory);

	// Read memory
	mA = readByte(cycles, memory, address);

	// Update status flags
	ldaUpdateStatus();
}

void CPU::ldaZpX(sdword &cycles, Memory &memory)
{
	// Fetch zero page address 
	word address = fetchAddrZeroPageX(cycles, memory);

	// Read memory
	mA = readByte(cycles, memory, address);

	// Update status flags
	ldaUpdateStatus();
}

void CPU::ldaAbs(sdword &cycles, Memory &memory)
{
	// Fetch absolute address
	word address = fetchAddrAbsolute(cycles, memory);

	// Read value
	mA = readByte(cycles, memory, address);

	// Update status flags
	ldaUpdateStatus();
}

void CPU::ldaAbsX(sdword &cycles, Memory &memory)
{
	// Fetch absolute address
	bool hasPageCrossed;
	word address = fetchAddrAbsoluteX(cycles, memory, hasPageCrossed);
	if (hasPageCrossed)
		cycles--;

	// Read value
	mA = readByte(cycles, memory, address);

	// Update status flags
	ldaUpdateStatus();
}

void CPU::ldaAbsY(sdword &cycles, Memory &memory)
{
	// Fetch absolute address
	bool hasPageCrossed;
	word address = fetchAddrAbsoluteY(cycles, memory, hasPageCrossed);
	if (hasPageCrossed)
		cycles--;

	// Read value
	mA = readByte(cycles, memory, address);

	// Update status flags
	ldaUpdateStatus();
}

void CPU::ldaIndX(sdword &cycles, Memory &memory)
{
	// Fetch indirectly address
	word targetAddress = fetchAddrIndirectX(cycles, memory);

	// Read value
	mA = readByte(cycles, memory, targetAddress);

	// Update status flags
	ldaUpdateStatus();
}

void CPU::ldaIndY(sdword &cycles, Memory &memory)
{
	// Fetch indirectly address
	bool hasPageCrossed;
	word targetAddress = fetchAddrIndirectY(cycles, memory, hasPageCrossed);
	if (hasPageCrossed)
		cycles--;

	// Read value
	mA = readByte(cycles, memory, targetAddress);

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
