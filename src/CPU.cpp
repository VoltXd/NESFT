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
	mB = 1; // Assumed to be 1, handled by operations
	mU = 1;
	mV = 0;
	mN = 0;

	// Reset memory
	memory.initialise();
}

sdword CPU::execute(sdword cycles, Memory &memory)
{
	sdword cyclesRequested = cycles;
	bool hasPageCrossed;
	while (cycles > 0)
	{
		// Fetch 
		byte instructionOpcode = fetchByte(cycles, memory);
		instruction_t instruction = INSTRUCTION_LUT[instructionOpcode];
		
		// Get address
		word address = fetchAddr(cycles, memory, instruction.addrMode, hasPageCrossed);
		
		// Execute
		executeInstruction(cycles, memory, instruction, address, hasPageCrossed);
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

void CPU::writeByte(sdword &cycles, Memory &memory, word address, byte value)
{
	// Write value into memory & decrement cycles counter
	memory[address] = value;
	cycles--;
}

void CPU::stackPush(sdword &cycles, Memory &memory, byte value)
{
	// Store value on to the stack pointer's position
	memory[SP_PAGE_OFFSET | mSp] = value;

	// Decrement stack pointer & cycles count
	mSp--;
	cycles--;
}

byte CPU::stackPull(sdword &cycles, Memory &memory)
{
	// Increment stack pointer & cycles count
	mSp++;
	cycles--;

	// Load value from the stack pointer's position
	byte value = memory[SP_PAGE_OFFSET | mSp];
	cycles--;

	return value;
}

word CPU::fetchAddr(sdword &cycles, Memory &memory, AddressingMode addrMode, bool &hasPageCrossed)
{
	word address;
	hasPageCrossed = false;
	switch (addrMode)
	{
	case AddressingMode::Implicit:
	{
		// Nothing to do... 
		address = 0x0000;
	} break;

	case AddressingMode::Accumulator:
	{
		// Nothing to do... 
		address = 0x0000;
	} break;

	case AddressingMode::Immediate:
	{
		// Just return PC and increment 
		address = mPc;
		mPc++;
	} break;

	case AddressingMode::ZeroPage:
	{
		// Fetch zero page address 
		address = fetchByte(cycles, memory);
	} break;

	case AddressingMode::ZeroPageX:
	{
		// Fetch zero page address 
		address = fetchByte(cycles, memory);
		
		// Add the value of register X to address, wraps in case of overflow
		address = (address + mX) & 0x00FF;
		cycles--;
	} break;

	case AddressingMode::ZeroPageY:
	{
		// Fetch zero page address 
		address = fetchByte(cycles, memory);
		
		// Add the value of register Y to address, wraps in case of overflow
		address = (address + mY) & 0x00FF;
		cycles--;
	} break;

	case AddressingMode::Relative:
	{
		// Fetch address offset
		// WARNING: the offset is signed, byte is a 8 bits unsigned data type
		byte offset = fetchByte(cycles, memory);
		sbyte signedOffset = *(sbyte*)&offset; 		// Pointer magic ;)

		// Calculate relative address
		address = mPc + signedOffset;
	} break;

	case AddressingMode::Absolute:
	{
		// Fetch absolute address
		address = fetchWord(cycles, memory);
	} break;

	case AddressingMode::AbsoluteX:
	{
		// Fetch absolute address
		address = fetchWord(cycles, memory);
		byte prevMsb = (address & 0xFF00) >> 8;

		// Add value of register X -> Wraps to zero page if needed
		address = ((uint32_t)address + mX) & 0x0000FFFFF;
		byte msb = (address & 0xFF00) >> 8;

		// If we go to the next page, some instruction will add one more cycle
		hasPageCrossed = msb != prevMsb;
	} break;

	case AddressingMode::AbsoluteY:
	{
		// Fetch absolute address
		address = fetchWord(cycles, memory);
		byte prevMsb = (address & 0xFF00) >> 8;

		// Add value of register Y -> Wraps to zero page if needed
		address = ((uint32_t)address + mY) & 0x0000FFFFF;
		byte msb = (address & 0xFF00) >> 8;

		// If we go to the next page, some instruction will add one more cycle
		hasPageCrossed = msb != prevMsb;
	} break;

	case AddressingMode::Indirect:
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
		address = ((word)targetAddressMsb << 8) | targetAddressLsb;
	} break;

	case AddressingMode::IndirectX:
	{
		// Fetch zero page address
		address = fetchByte(cycles, memory);

		// Add value of X register to the address
		address = (address + mX) & 0x00FF;
		cycles--;

		// Fetch absolute address LSB 
		byte targetAddressLsb = readByte(cycles, memory, address);
		// Fetch absolute address MSB 
		address = (address + 1 ) & 0x00FF;
		byte targetAddressMsb = readByte(cycles, memory, address);

		// Combine LSB & MSB
		address = ((word)targetAddressMsb << 8) | targetAddressLsb;
	} break;

	case AddressingMode::IndirectY:
	{
		// Fetch zero page address
		word zpAddress = fetchByte(cycles, memory);

		// Fetch absolute address LSB 
		byte targetAddressLsb = readByte(cycles, memory, zpAddress);
		// Fetch absolute address MSB 
		zpAddress = (zpAddress + 1 ) & 0x00FF;
		byte targetAddressMsb = readByte(cycles, memory, zpAddress);

		// Combine LSB & MSB
		address = ((word)targetAddressMsb << 8) | targetAddressLsb;

		// Add Y to target address
		address = ((uint32_t)address + mY) & 0xFFFF;
		
		// If we go to the next page, some instruction will add one more cycle
		hasPageCrossed = targetAddressMsb != (address & 0xFF00) >> 8;
	} break;
	
	default:
		// This can't happen... just crash the program if it access this case
		// TODO: Replace replace with an error window
		std::cout << __FILE__ 
				  << ':' 
				  << __LINE__ 
				  << ": error: accessed impossible default case. Just tell the programmer he's dumb if it occurs..."; 
		exit(EXIT_FAILURE);
		break;
	}

	return address;
}

void CPU::executeInstruction(sdword &cycles, Memory &memory, instruction_t instruction, word address, bool hasPageCrossed)
{
	Operation operation = instruction.operation;
	AddressingMode addrMode = instruction.addrMode;
	byte opcode = instruction.opcode;

	switch (operation)
	{
	case Operation::ADC:
		adc(cycles, memory, address, hasPageCrossed);
		break;

	case Operation::JSR:
		jsr(cycles, memory, address);
		break;

	case Operation::LDA:
		lda(cycles, memory, address, hasPageCrossed);
		break;

	case Operation::LDX:
		ldx(cycles, memory, address, hasPageCrossed);
		break;

	case Operation::LDY:
		ldy(cycles, memory, address, hasPageCrossed);
		break;

	case Operation::PHA:
		pha(cycles, memory);
		break;

	case Operation::PHP:
		php(cycles, memory);
		break;

	case Operation::PLA:
		pla(cycles, memory);
		break;

	case Operation::PLP:
		plp(cycles, memory);
		break;

	case Operation::SBC:
		sbc(cycles, memory, address, hasPageCrossed);
		break;

	case Operation::STA:
		sta(cycles, memory, address, addrMode);
		break;

	case Operation::STX:
		stx(cycles, memory, address);
		break;

	case Operation::STY:
		sty(cycles, memory, address);
		break;

	case Operation::TAX:
		tax(cycles);
		break;

	case Operation::TAY:
		tay(cycles);
		break;

	case Operation::TSX:
		tsx(cycles);
		break;

	case Operation::TXA:
		txa(cycles);
		break;

	case Operation::TXS:
		txs(cycles);
		break;

	case Operation::TYA:
		tya(cycles);
		break;

	default:
		std::cout << "Operation not handled: 0x" << std::hex << std::uppercase << (int)opcode << std::dec << std::endl;
		cycles--;
		break;
	}
}

void CPU::adc(sdword &cycles, Memory &memory, word address, bool hasPageCrossed)
{
	// Save previous Accumulator state, for status update
	byte previousA = mA;

	// Read value from memory
	byte memValue = readByte(cycles, memory, address);

	// Perform add with carry
	word newA = mA + memValue + mC;
	mA = newA & 0x00FF;

	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		cycles--;
	
	// Update status flags
	adcUpdateStatus(newA, previousA, memValue);
}

void CPU::jsr(sdword &cycles, Memory &memory, word subroutineAddress)
{
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

void CPU::lda(sdword &cycles, Memory &memory, word address, bool hasPageCrossed)
{
	// Read memory
	mA = readByte(cycles, memory, address);
	
	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		cycles--;

	// Update status flags
	ldaUpdateStatus();
}

void CPU::ldx(sdword &cycles, Memory &memory, word address, bool hasPageCrossed)
{
	// Read memory
	mX = readByte(cycles, memory, address);
	
	// Decrement cycles count if page crossing (abs.Y)
	if (hasPageCrossed)
		cycles--;

	// Update status flags
	ldxUpdateStatus();
}

void CPU::ldy(sdword &cycles, Memory &memory, word address, bool hasPageCrossed)
{
	// Read memory
	mY = readByte(cycles, memory, address);
	
	// Decrement cycles count if page crossing (abs.Y)
	if (hasPageCrossed)
		cycles--;

	// Update status flags
	ldyUpdateStatus();
}

void CPU::pha(sdword &cycles, Memory& memory)
{
	// Push A to Stack (simulate A read with a clock cycle)
	cycles--;
	stackPush(cycles, memory, mA);
}

void CPU::php(sdword &cycles, Memory &memory)
{
	// Push processor status to stack (simulate PStatus read with a clock cycle)
	byte status = getProcessorStatus();
	cycles--;

	stackPush(cycles, memory, status);
}

void CPU::pla(sdword &cycles, Memory &memory)
{
	// Pull A (+ 1 dead cycle, can't figure out why...)
	mA = stackPull(cycles, memory);
	cycles--;

	// Update status flags
	plaUpdateStatus();
}

void CPU::plp(sdword &cycles, Memory &memory)
{
	// Pull Processor status (+ 1 dead cycle, can't figure out why...)
	byte processorStatus = stackPull(cycles, memory);
	cycles--;

	// Update status flags
	setProcessorStatus(processorStatus);
}

void CPU::sbc(sdword &cycles, Memory &memory, word address, bool hasPageCrossed)
{
	// Save previous Accumulator state, for status update
	byte previousA = mA;

	// Read value from memory
	byte memValue = readByte(cycles, memory, address);

	// Perform substract with carry
	word newA = mA - memValue - ((word)1 - mC);
	mA = newA & 0x00FF;

	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		cycles--;
	
	// Update status flags
	sbcUpdateStatus(newA, previousA, memValue);
}

void CPU::sta(sdword &cycles, Memory &memory, word address, AddressingMode addrMode)
{
	// A -> mem[addr]
	writeByte(cycles, memory, address, mA);	

	// Dummy cycle for (abs.X, abs.Y, ind.Y)
	if (addrMode == AddressingMode::AbsoluteX ||
		addrMode == AddressingMode::AbsoluteY ||
		addrMode == AddressingMode::IndirectY)
		cycles--;
}

void CPU::stx(sdword &cycles, Memory &memory, word address)
{
	// X -> mem[addr]
	writeByte(cycles, memory, address, mX);	
}

void CPU::sty(sdword &cycles, Memory &memory, word address)
{
	// Y -> mem[addr]
	writeByte(cycles, memory, address, mY);	
}

void CPU::tax(sdword &cycles)
{
	// A -> X
	mX = mA;
	cycles--;

	// Update status flags
	taxUpdateStatus();
}

void CPU::tay(sdword &cycles)
{
	// A -> Y
	mY = mA;
	cycles--;

	// Update status flags
	tayUpdateStatus();
}

void CPU::tsx(sdword &cycles)
{
	// SP -> X
	mX = mSp;
	cycles--;

	// Update status flags
	tsxUpdateStatus();
}

void CPU::txa(sdword &cycles)
{
	// X -> A
	mA = mX;
	cycles--;

	// Update status flags
	txaUpdateStatus();
}

void CPU::txs(sdword &cycles)
{
	// X -> SP
	mSp = mX;
	cycles--;
}

void CPU::tya(sdword &cycles)
{
	// Y -> A
	mA = mY;
	cycles--;

	// Update status flags
	tyaUpdateStatus();
}

byte CPU::getProcessorStatus() const
{
	byte processorStatus = 0x00;
	processorStatus |= (mC & 0x01) << 0 |
	                   (mZ & 0x01) << 1 |
	                   (mI & 0x01) << 2 |
	                   (mD & 0x01) << 3 |
	                   (mB & 0x01) << 4 |
	                   (mU & 0x01) << 5 |
	                   (mV & 0x01) << 6 |
	                   (mN & 0x01) << 7;

	return processorStatus;
}

void CPU::setProcessorStatus(byte processorStatus)
{
	mC = (processorStatus >> 0) & 0x01;
	mZ = (processorStatus >> 1) & 0x01;
	mI = (processorStatus >> 2) & 0x01;
	mD = (processorStatus >> 3) & 0x01;
	mV = (processorStatus >> 6) & 0x01;
	mN = (processorStatus >> 7) & 0x01;
}

void CPU::adcUpdateStatus(word newA, byte operandA, byte operandM)
{
	// Update C, Z, V, N
	mC = (newA & 0x0100) != 0    ? 1 : 0;
	mZ = mA == 0                 ? 1 : 0;
	mV = (operandA ^ mA) &
	     (operandM ^ mA) & 0x80  ? 1 : 0;
    mN = (mA & 0b1000'0000) != 0 ? 1 : 0;
}

void CPU::ldaUpdateStatus()
{
	// Only Z & N flags need to be updated
	mZ = mA == 0          ? 1 : 0;
    mN = mA & 0b1000'0000 ? 1 : 0;
}

void CPU::ldxUpdateStatus()
{
	// Only Z & N flags need to be updated
	mZ = mX == 0          ? 1 : 0;
    mN = mX & 0b1000'0000 ? 1 : 0;
}

void CPU::ldyUpdateStatus()
{
	// Only Z & N flags need to be updated
	mZ = mY == 0          ? 1 : 0;
    mN = mY & 0b1000'0000 ? 1 : 0;
}

void CPU::plaUpdateStatus()
{
	// Only Z & N flags need to be updated
	mZ = mA == 0          ? 1 : 0;
    mN = mA & 0b1000'0000 ? 1 : 0;
}

void CPU::sbcUpdateStatus(word newA, byte operandA, byte operandM)
{
	// Update C, Z, V, N
	mC = (newA & 0x0100) != 0    ? 0 : 1;
	mZ = mA == 0                 ? 1 : 0;
	mV = ( operandA ^ mA) &
	     (~operandM ^ mA) & 0x80 ? 1 : 0;
    mN = (mA & 0b1000'0000) != 0 ? 1 : 0;
}

void CPU::taxUpdateStatus()
{
	// Only Z & N flags need to be updated
	mZ = mX == 0          ? 1 : 0;
    mN = mX & 0b1000'0000 ? 1 : 0;
}

void CPU::tayUpdateStatus()
{
	// Only Z & N flags need to be updated
	mZ = mY == 0          ? 1 : 0;
    mN = mY & 0b1000'0000 ? 1 : 0;
}

void CPU::tsxUpdateStatus()
{
	// Only Z & N flags need to be updated
	mZ = mX == 0          ? 1 : 0;
    mN = mX & 0b1000'0000 ? 1 : 0;
}

void CPU::txaUpdateStatus()
{
	// Only Z & N flags need to be updated
	mZ = mA == 0          ? 1 : 0;
    mN = mA & 0b1000'0000 ? 1 : 0;
}

void CPU::tyaUpdateStatus()
{
	// Only Z & N flags need to be updated
	mZ = mA == 0          ? 1 : 0;
    mN = mA & 0b1000'0000 ? 1 : 0;
}
