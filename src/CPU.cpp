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

		// Branch to another page => additionnal cycles (XOR magic, too lazy to store MSBs)
		hasPageCrossed = ((mPc ^ address) & 0xFF00) != 0;
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
		// the MSB is not incremented.
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

	case Operation::AND:
		and_(cycles, memory, address, hasPageCrossed);
		break;

	case Operation::ASL:
		asl(cycles, memory, address, addrMode);
		break;

	case Operation::BCC:
		bcc(cycles, address, hasPageCrossed);
		break;

	case Operation::BCS:
		bcs(cycles, address, hasPageCrossed);
		break;

	case Operation::BEQ:
		beq(cycles, address, hasPageCrossed);
		break;

	case Operation::BIT:
		bit(cycles, memory, address);
		break;

	case Operation::BMI:
		bmi(cycles, address, hasPageCrossed);
		break;

	case Operation::BNE:
		bne(cycles, address, hasPageCrossed);
		break;

	case Operation::BPL:
		bpl(cycles, address, hasPageCrossed);
		break;

	case Operation::BVC:
		bvc(cycles, address, hasPageCrossed);
		break;

	case Operation::BVS:
		bvs(cycles, address, hasPageCrossed);
		break;

	case Operation::CLC:
		clc(cycles);
		break;

	case Operation::CLD:
		cld(cycles);
		break;

	case Operation::CLI:
		cli(cycles);
		break;

	case Operation::CLV:
		clv(cycles);
		break;

	case Operation::CMP:
		cmp(cycles, memory, address, hasPageCrossed);
		break;

	case Operation::CPX:
		cpx(cycles, memory, address, hasPageCrossed);
		break;

	case Operation::CPY:
		cpy(cycles, memory, address, hasPageCrossed);
		break;

	case Operation::DEX:
		dex(cycles);
		break;

	case Operation::DEY:
		dey(cycles);
		break;

	case Operation::DEC:
		dec(cycles, memory, address, addrMode);
		break;

	case Operation::EOR:
		eor(cycles, memory, address, hasPageCrossed);
		break;

	case Operation::INC:
		inc(cycles, memory, address, addrMode);
		break;

	case Operation::INX:
		inx(cycles);
		break;

	case Operation::INY:
		iny(cycles);
		break;

	case Operation::JMP:
		jmp(address);
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

	case Operation::LSR:
		lsr(cycles, memory, address, addrMode);
		break;

	case Operation::ORA:
		ora(cycles, memory, address, hasPageCrossed);
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

	case Operation::ROL:
		rol(cycles, memory, address, addrMode);
		break;

	case Operation::ROR:
		ror(cycles, memory, address, addrMode);
		break;

	case Operation::RTS:
		rts(cycles, memory);
		break;

	case Operation::SBC:
		sbc(cycles, memory, address, hasPageCrossed);
		break;

	case Operation::SEC:
		sec(cycles);
		break;

	case Operation::SED:
		sed(cycles);
		break;

	case Operation::SEI:
		sei(cycles);
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

void CPU::and_(sdword &cycles, Memory &memory, word address, bool hasPageCrossed)
{
	// Read value from memory
	byte memValue = readByte(cycles, memory, address);

	// Perform '&' mask
	mA &= memValue;

	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		cycles--;

	// Update status flags
	andUpdateStatus();
}

void CPU::asl(sdword &cycles, Memory &memory, word address, AddressingMode addrMode)
{
	// Read value from memory or accumulator
	byte previousValue;
	if (addrMode == AddressingMode::Accumulator)
		previousValue = mA;
	else
		previousValue = readByte(cycles, memory, address);

	// Shift bytes
	byte newValue = previousValue << 1;
	cycles--;

	// Write back value where it belongs
	if (addrMode == AddressingMode::Accumulator)
		mA = newValue;
	else
		writeByte(cycles, memory, address, newValue);

	// Dummy cycle on Abs.X writing
	if (addrMode == AddressingMode::AbsoluteX)
		cycles--;

	// Update status flags
	aslUpdateStatus(previousValue, newValue);
}

void CPU::bcc(sdword &cycles, word address, bool hasPageCrossed)
{
	// Branch condition
	if (!mC)
	{
		mPc = address;
		cycles--;

		if (hasPageCrossed)
			cycles--;
	}
}

void CPU::bcs(sdword &cycles, word address, bool hasPageCrossed)
{
	// Branch condition
	if (mC)
	{
		mPc = address;
		cycles--;

		if (hasPageCrossed)
			cycles--;
	}
}

void CPU::beq(sdword &cycles, word address, bool hasPageCrossed)
{
	// Branch condition
	if (mZ)
	{
		mPc = address;
		cycles--;

		if (hasPageCrossed)
			cycles--;
	}
}

void CPU::bit(sdword &cycles, Memory &memory, word address)
{
	// Read value from memory
	byte memValue = readByte(cycles, memory, address);

	// Bit tests
	mZ = (mA & memValue) == 0          ? 1 : 0;
	mV = (memValue & 0b0100'0000) != 0 ? 1 : 0;
	mN = (memValue & 0b1000'0000) != 0 ? 1 : 0;
}

void CPU::bmi(sdword &cycles, word address, bool hasPageCrossed)
{
	// Branch condition
	if (mN)
	{
		mPc = address;
		cycles--;

		if (hasPageCrossed)
			cycles--;
	}
}

void CPU::bne(sdword &cycles, word address, bool hasPageCrossed)
{
	// Branch condition
	if (!mZ)
	{
		mPc = address;
		cycles--;

		if (hasPageCrossed)
			cycles--;
	}
}

void CPU::bpl(sdword &cycles, word address, bool hasPageCrossed)
{
	// Branch condition
	if (!mN)
	{
		mPc = address;
		cycles--;

		if (hasPageCrossed)
			cycles--;
	}
}

void CPU::bvc(sdword &cycles, word address, bool hasPageCrossed)
{
	// Branch condition
	if (!mV)
	{
		mPc = address;
		cycles--;

		if (hasPageCrossed)
			cycles--;
	}
}

void CPU::bvs(sdword &cycles, word address, bool hasPageCrossed)
{
	// Branch condition
	if (mV)
	{
		mPc = address;
		cycles--;

		if (hasPageCrossed)
			cycles--;
	}
}

void CPU::clc(sdword &cycles)
{
	// Update status
	mC = 0;
	cycles--;
}

void CPU::cld(sdword &cycles)
{
	// Update status
	mD = 0;
	cycles--;
}

void CPU::cli(sdword &cycles)
{
	// Update status
	mI = 0;
	cycles--;
}

void CPU::clv(sdword &cycles)
{
	// Update status
	mV = 0;
	cycles--;
}

void CPU::cmp(sdword &cycles, Memory &memory, word address, bool hasPageCrossed)
{
	// Read value from memory
	byte memValue = readByte(cycles, memory, address);

	// Perform substract to compare & update flags
	word comparisonValue = mA - memValue;
	mC = (comparisonValue & 0xFF00) == 0 ? 1 : 0;
	mZ = comparisonValue == 0            ? 1 : 0;
	mN = (comparisonValue & 0x0080) != 0 ? 1 : 0;

	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		cycles--;
}

void CPU::cpx(sdword &cycles, Memory &memory, word address, bool hasPageCrossed)
{
	// Read value from memory
	byte memValue = readByte(cycles, memory, address);

	// Perform substract to compare & update flags
	word comparisonValue = mX - memValue;
	mC = (comparisonValue & 0xFF00) == 0 ? 1 : 0;
	mZ = comparisonValue == 0            ? 1 : 0;
	mN = (comparisonValue & 0x0080) != 0 ? 1 : 0;

	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		cycles--;
}

void CPU::cpy(sdword &cycles, Memory &memory, word address, bool hasPageCrossed)
{
	// Read value from memory
	byte memValue = readByte(cycles, memory, address);

	// Perform substract to compare & update flags
	word comparisonValue = mY - memValue;
	mC = (comparisonValue & 0xFF00) == 0 ? 1 : 0;
	mZ = comparisonValue == 0            ? 1 : 0;
	mN = (comparisonValue & 0x0080) != 0 ? 1 : 0;

	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		cycles--;
}

void CPU::dec(sdword &cycles, Memory &memory, word address, AddressingMode addrMode)
{
	// Read value from memory
	byte memValue = readByte(cycles, memory, address);

	// Increment value
	memValue--;
	cycles--;
	
	// Write value to memory
	writeByte(cycles, memory, address, memValue);

	// Dummy cycle for Abs.X
	if (addrMode == AddressingMode::AbsoluteX)
		cycles--;

	// Update status flags
	decUpdateStatus(memValue);
}

void CPU::dex(sdword &cycles)
{
	// Decrement X register
	mX--;
	cycles--;

	// Update status flags
	dexUpdateStatus();
}

void CPU::dey(sdword &cycles)
{
	// Decrement Y register
	mY--;
	cycles--;

	// Update status flags
	deyUpdateStatus();
}

void CPU::eor(sdword &cycles, Memory &memory, word address, bool hasPageCrossed)
{
	// Read value from memory
	byte memValue = readByte(cycles, memory, address);

	// Perform '^' mask
	mA ^= memValue;

	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		cycles--;

	// Update status flags
	eorUpdateStatus();
}

void CPU::inc(sdword &cycles, Memory &memory, word address, AddressingMode addrMode)
{
	// Read value from memory
	byte memValue = readByte(cycles, memory, address);

	// Increment value
	memValue++;
	cycles--;
	
	// Write value to memory
	writeByte(cycles, memory, address, memValue);

	// Dummy cycle for Abs.X
	if (addrMode == AddressingMode::AbsoluteX)
		cycles--;

	// Update status flags
	incUpdateStatus(memValue);
}

void CPU::inx(sdword &cycles)
{
	// Increment X register
	mX++;
	cycles--;

	// Update status flags
	inxUpdateStatus();
}

void CPU::iny(sdword &cycles)
{
	// Increment Y register
	mY++;
	cycles--;

	// Update status flags
	inyUpdateStatus();
}

void CPU::jmp(word address)
{
	// Jump to location
	mPc = address;
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

void CPU::lsr(sdword &cycles, Memory &memory, word address, AddressingMode addrMode)
{
	// Read value from memory or accumulator
	byte previousValue;
	if (addrMode == AddressingMode::Accumulator)
		previousValue = mA;
	else
		previousValue = readByte(cycles, memory, address);

	// Shift bytes
	byte newValue = previousValue >> 1;
	cycles--;

	// Write back value where it belongs
	if (addrMode == AddressingMode::Accumulator)
		mA = newValue;
	else
		writeByte(cycles, memory, address, newValue);

	// Dummy cycle on Abs.X writing
	if (addrMode == AddressingMode::AbsoluteX)
		cycles--;

	// Update status flags
	lsrUpdateStatus(previousValue, newValue);
}

void CPU::ora(sdword &cycles, Memory &memory, word address, bool hasPageCrossed)
{
	// Read value from memory
	byte memValue = readByte(cycles, memory, address);

	// Perform '|' mask
	mA |= memValue;

	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		cycles--;

	// Update status flags
	oraUpdateStatus();
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

void CPU::rol(sdword &cycles, Memory &memory, word address, AddressingMode addrMode)
{
	// Read value from memory or accumulator
	byte previousValue;
	if (addrMode == AddressingMode::Accumulator)
		previousValue = mA;
	else
		previousValue = readByte(cycles, memory, address);

	// Shift bytes
	byte newValue = (previousValue << 1) | mC;
	cycles--;

	// Write back value where it belongs
	if (addrMode == AddressingMode::Accumulator)
		mA = newValue;
	else
		writeByte(cycles, memory, address, newValue);

	// Dummy cycle on Abs.X writing
	if (addrMode == AddressingMode::AbsoluteX)
		cycles--;

	// Update status flags
	rolUpdateStatus(previousValue, newValue);
}

void CPU::ror(sdword &cycles, Memory &memory, word address, AddressingMode addrMode)
{
	// Read value from memory or accumulator
	byte previousValue;
	if (addrMode == AddressingMode::Accumulator)
		previousValue = mA;
	else
		previousValue = readByte(cycles, memory, address);

	// Shift bytes
	byte newValue = (previousValue >> 1) | (mC << 7);
	cycles--;

	// Write back value where it belongs
	if (addrMode == AddressingMode::Accumulator)
		mA = newValue;
	else
		writeByte(cycles, memory, address, newValue);

	// Dummy cycle on Abs.X writing
	if (addrMode == AddressingMode::AbsoluteX)
		cycles--;

	// Update status flags
	rorUpdateStatus(previousValue, newValue);
}

void CPU::rts(sdword &cycles, Memory &memory)
{
	// Pull Program counter -1 (return address) to stack (2 cycles)
	byte returnAddressLsb = stackPull(cycles, memory);
	byte returnAddressMsb = stackPull(cycles, memory);
	word returnAddress = ((word)returnAddressMsb << 8) | returnAddressLsb;

	// Set Program counter + 1 (1 cycle)
	mPc = returnAddress + 1;
	cycles--;
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

void CPU::sec(sdword &cycles)
{
	// Update status
	mC = 1;
	cycles--;
}

void CPU::sed(sdword &cycles)
{
	// Update status
	mD = 1;
	cycles--;
}

void CPU::sei(sdword &cycles)
{
	// Update status
	mI = 1;
	cycles--;
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

void CPU::andUpdateStatus()
{
	// Only Z & N flags need to be updated
	mZ = mA == 0          ? 1 : 0;
    mN = mA & 0b1000'0000 ? 1 : 0;
}

void CPU::aslUpdateStatus(byte previousValue, byte newValue)
{
	mC = (previousValue & 0x80) != 0 ? 1 : 0;
	mZ = newValue == 0               ? 1 : 0;
	mN = (newValue & 0x80) != 0      ? 1 : 0;
}

void CPU::decUpdateStatus(byte memValue)
{
	// Only Z & N flags need to be updated
	mZ = memValue == 0          ? 1 : 0;
	mN = memValue & 0b1000'0000 ? 1 : 0;
}

void CPU::dexUpdateStatus()
{
	// Only Z & N flags need to be updated
	mZ = mX == 0          ? 1 : 0;
	mN = mX & 0b1000'0000 ? 1 : 0;
}

void CPU::deyUpdateStatus()
{
	// Only Z & N flags need to be updated
	mZ = mY == 0          ? 1 : 0;
	mN = mY & 0b1000'0000 ? 1 : 0;
}

void CPU::eorUpdateStatus()
{
	// Only Z & N flags need to be updated
	mZ = mA == 0          ? 1 : 0;
    mN = mA & 0b1000'0000 ? 1 : 0;
}

void CPU::incUpdateStatus(byte memValue)
{
	// Only Z & N flags need to be updated
	mZ = memValue == 0          ? 1 : 0;
	mN = memValue & 0b1000'0000 ? 1 : 0;
}

void CPU::inxUpdateStatus()
{
	// Only Z & N flags need to be updated
	mZ = mX == 0          ? 1 : 0;
	mN = mX & 0b1000'0000 ? 1 : 0;
}

void CPU::inyUpdateStatus()
{
	// Only Z & N flags need to be updated
	mZ = mY == 0          ? 1 : 0;
	mN = mY & 0b1000'0000 ? 1 : 0;
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

void CPU::lsrUpdateStatus(byte previousValue, byte newValue)
{
	mC = (previousValue & 0x01) != 0 ? 1 : 0;
	mZ = newValue == 0               ? 1 : 0;
	mN = 0;
}

void CPU::oraUpdateStatus()
{
	// Only Z & N flags need to be updated
	mZ = mA == 0          ? 1 : 0;
    mN = mA & 0b1000'0000 ? 1 : 0;
}

void CPU::plaUpdateStatus()
{
	// Only Z & N flags need to be updated
	mZ = mA == 0          ? 1 : 0;
    mN = mA & 0b1000'0000 ? 1 : 0;
}

void CPU::rolUpdateStatus(byte previousValue, byte newValue)
{
	mC = (previousValue & 0x80) != 0 ? 1 : 0;
	mZ = newValue == 0               ? 1 : 0;
	mN = (newValue & 0x80) != 0      ? 1 : 0;
}

void CPU::rorUpdateStatus(byte previousValue, byte newValue)
{
	mC = (previousValue & 0x01) != 0 ? 1 : 0;
	mZ = newValue == 0               ? 1 : 0;
	mN = (newValue & 0x80) != 0      ? 1 : 0;
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
