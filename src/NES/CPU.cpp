#include "NES/CPU.hpp"

#include <iostream>
#include <iomanip>
#include "NES/Toolbox.hpp"

s32 CPU::reset(Memory& memory)
{
	// Cycles variables
	constexpr s32 resetCycles = 7;
	s32 cycles = resetCycles;

	// Reset registers
	mPc = RESET_VECTOR_LSB;
	mSp = SP_RESET;

	mA = 0;
	mX = 0;
	mY = 0;

	mC = 0;
	mZ = 0;
	mI = 1;
	mD = 0;
	mB = 1; // Assumed to be 1, handled by operations
	mU = 1;
	mV = 0;
	mN = 0;

	mPreviousI = 1;
	mIsIDelayed = false;

	// Push stack 2 times
	mSp -= 2;

	// Get first instruction address
	mPc = fetchWord(cycles, memory);

	return resetCycles;
}

s32 CPU::irq(Memory &memory)
{
	// Cycles variables
	constexpr s32 interruptCycles = 7;
	s32 cycles = 7;

	// Ignore interrupt if I flag is set
	if (mIsIDelayed)
	{
		if (mPreviousI)
			return 0;
	}
	else if (mI)
	{
		return 0;
	}

	// Log IRQ
	cpuLogIrq();

	// Push program counter & processor status
	u8 pcLsb = mPc & 0x00FF;
	u8 pcMsb = ((u16)mPc & 0xFF00) >> 8;
	mB = 0;
	u8 processorStatus = getProcessorStatus();
	stackPush(cycles, memory, pcMsb);
	stackPush(cycles, memory, pcLsb);
	stackPush(cycles, memory, processorStatus);

	// Fetch interrupt address
	u8 interruptAddressLsb = readByte(cycles, memory, IRQ_VECTOR_LSB); 
	u8 interruptAddressMsb = readByte(cycles, memory, IRQ_VECTOR_MSB); 
	mI = 1;
	mPreviousI = 1;
	mIsIDelayed = false;

	// Program counter update
	mPc = ((u16)interruptAddressMsb << 8) | interruptAddressLsb;

	// Log end
	cpuLogEnd();

	return interruptCycles;
}

s32 CPU::nmi(Memory &memory)
{
	// Cycles variables
	constexpr s32 interruptCycles = 7;
	s32 cycles = 7;

	// Log NMI
	cpuLogNmi();

	// Push program counter & processor status
	u8 pcLsb = mPc & 0x00FF;
	u8 pcMsb = ((u16)mPc & 0xFF00) >> 8;
	mB = 0;
	u8 processorStatus = getProcessorStatus();
	stackPush(cycles, memory, pcMsb);
	stackPush(cycles, memory, pcLsb);
	stackPush(cycles, memory, processorStatus);

	// Fetch interrupt address
	u8 interruptAddressLsb = readByte(cycles, memory, NMI_VECTOR_LSB); 
	u8 interruptAddressMsb = readByte(cycles, memory, NMI_VECTOR_MSB); 
	mI = 1;

	// Program counter update
	mPc = ((u16)interruptAddressMsb << 8) | interruptAddressLsb;

	// Log end
	cpuLogEnd();
	
	return interruptCycles;
}

s32 CPU::execute(s32 cycles, Memory &memory)
{
	s32 cyclesRequested = cycles;
	bool hasPageCrossed;
	while (cycles > 0)
	{
		// Log start state
		cpuLog();

		// Reset I delay
		mIsIDelayed = false;
		
		// Fetch 
		u8 instructionOpcode = fetchByte(cycles, memory);
		instruction_t instruction = INSTRUCTION_LUT[instructionOpcode];

		// Log disassembled instruction
		cpuLogDisassembly(instruction.str);

		// Get address
		u16 dummyAddress;
		u16 address = fetchAddr(cycles, memory, instruction.addrMode, dummyAddress, hasPageCrossed);
		
		// Execute
		executeInstruction(cycles, memory, instruction, address, dummyAddress, hasPageCrossed);

		// Log end
		cpuLogEnd();
	}

	return cyclesRequested - cycles;
}

u8 CPU::fetchByte(s32& cycles, Memory &memory)
{
	// Get one byte from memory & increment program counter
	u8 data = memory.cpuRead(mPc);
	mPc++;

	// Decrement remaining cycles counter
	cycles--;

	return data;
}

u16 CPU::fetchWord(s32 &cycles, Memory &memory)
{
	// 6502 => Little endian => 1st is LSB, 2nd is MSB
	// Get LSB from memory & increment program counter
	u16 data = memory.cpuRead(mPc);
	mPc++;
	cycles--;

	// Get MSB from memory & increment program counter
	data |= (u16)memory.cpuRead(mPc) << 8;
	mPc++;
	cycles--;

	return data;
}

u8 CPU::readByte(s32 &cycles, Memory &memory, u16 address)
{
	// Get one byte from memory & decrement cycles counter
	u8 data = memory.cpuRead(address);
	cycles--;

	return data;
}

void CPU::writeByte(s32 &cycles, Memory &memory, u16 address, u8 value)
{
	// Write value into memory & decrement cycles counter
	memory.cpuWrite(address, value);
	cycles--;
}

void CPU::stackPush(s32 &cycles, Memory &memory, u8 value)
{
	// Store value on to the stack pointer's position
	memory.cpuWrite(SP_PAGE_OFFSET | mSp, value);

	// Decrement stack pointer & cycles count
	mSp--;
	cycles--;
}

u8 CPU::stackPull(s32 &cycles, Memory &memory)
{
	// Increment stack pointer & cycles count
	mSp++;
	cycles--;

	// Load value from the stack pointer's position
	u8 value = memory.cpuRead(SP_PAGE_OFFSET | mSp);
	cycles--;

	return value;
}

u16 CPU::fetchAddr(s32 &cycles, Memory &memory, AddressingMode addrMode, u16& dummyAddress, bool &hasPageCrossed)
{
	u16 address;
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
		u8 offset = fetchByte(cycles, memory);
		s8 signedOffset = *(s8*)&offset; 		// Pointer magic ;)

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
		dummyAddress = fetchWord(cycles, memory);
		u8 prevMsb = (dummyAddress & 0xFF00) >> 8;

		// Add value of register X -> Wraps to zero page
		address = dummyAddress + mX;
		dummyAddress = (dummyAddress & 0xFF00) | ((dummyAddress & 0x00FF) + mX);
		u8 msb = (address & 0xFF00) >> 8;

		// If we go to the next page, some instruction will add one more cycle
		hasPageCrossed = msb != prevMsb;
	} break;

	case AddressingMode::AbsoluteY:
	{
		// Fetch absolute address
		dummyAddress = fetchWord(cycles, memory);
		u8 prevMsb = (dummyAddress & 0xFF00) >> 8;

		// Add value of register Y -> Wraps to zero page if needed
		address = dummyAddress + mY;
		dummyAddress = (dummyAddress & 0xFF00) | ((dummyAddress & 0x00FF) + mY);
		u8 msb = (address & 0xFF00) >> 8;

		// If we go to the next page, some instruction will add one more cycle
		hasPageCrossed = msb != prevMsb;
	} break;

	case AddressingMode::Indirect:
	{
		// Fetch indirect address
		u16 indirectAddress = fetchWord(cycles, memory);
		u8 indirectAddressLsb = indirectAddress & 0x00FF;
		u8 indirectAddressMsb = (indirectAddress & 0xFF00) >> 8;

		// Fetch target address
		// WARNING: according to https://www.nesdev.org/obelisk-6502-guide/reference.html#JMP,
		// if indirect address points to a page boundary, the LSB will wrap ($FF -> $00) but
		// the MSB is not incremented.
		u16 indirectAddressToMsb = ((u16)indirectAddressMsb << 8) | ((indirectAddressLsb + 1) & 0x00FF); 
		u8 targetAddressLsb = readByte(cycles, memory, indirectAddress);
		u8 targetAddressMsb = readByte(cycles, memory, indirectAddressToMsb);
		address = ((u16)targetAddressMsb << 8) | targetAddressLsb;
	} break;

	case AddressingMode::IndirectX:
	{
		// Fetch zero page address
		address = fetchByte(cycles, memory);

		// Add value of X register to the address
		address = (address + mX) & 0x00FF;
		cycles--;

		// Fetch absolute address LSB 
		u8 targetAddressLsb = readByte(cycles, memory, address);
		// Fetch absolute address MSB 
		address = (address + 1 ) & 0x00FF;
		u8 targetAddressMsb = readByte(cycles, memory, address);

		// Combine LSB & MSB
		address = ((u16)targetAddressMsb << 8) | targetAddressLsb;
	} break;

	case AddressingMode::IndirectY:
	{
		// Fetch zero page address
		u16 zpAddress = fetchByte(cycles, memory);

		// Fetch absolute address LSB 
		u8 targetAddressLsb = readByte(cycles, memory, zpAddress);
		// Fetch absolute address MSB 
		zpAddress = (zpAddress + 1 ) & 0x00FF;
		u8 targetAddressMsb = readByte(cycles, memory, zpAddress);

		// Combine LSB & MSB
		dummyAddress = ((u16)targetAddressMsb << 8) | targetAddressLsb;

		// Add Y to target address
		address = dummyAddress + mY;
		dummyAddress = (dummyAddress & 0xFF00) | ((dummyAddress & 0x00FF) + mY);
		
		// If we go to the next page, some instruction will add one more cycle
		hasPageCrossed = targetAddressMsb != ((address & 0xFF00) >> 8);
	} break;
	
	default:
		// This can't happen... just crash the program if it access this case
		std::cout << __FILE__ 
				  << ':' 
				  << __LINE__ 
				  << ": error: accessed impossible default case. Just tell the programmer he's dumb if it occurs..."; 
		exit(EXIT_FAILURE);
		break;
	}

	return address;
}

void CPU::executeInstruction(s32 &cycles, Memory &memory, instruction_t instruction, u16 address, u16 dummyAddress, bool hasPageCrossed)
{
	Operation operation = instruction.operation;
	AddressingMode addrMode = instruction.addrMode;
	u8 opcode = instruction.opcode;

	switch (operation)
	{
	case Operation::ADC:
		adc(cycles, memory, address, dummyAddress, hasPageCrossed);
		break;

	case Operation::AND:
		and_(cycles, memory, address, dummyAddress, hasPageCrossed);
		break;

	case Operation::ASL:
		asl(cycles, memory, address, dummyAddress, addrMode);
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

	case Operation::BRK:
		brk(cycles, memory);
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
		cmp(cycles, memory, address, dummyAddress, hasPageCrossed);
		break;

	case Operation::CPX:
		cpx(cycles, memory, address, dummyAddress, hasPageCrossed);
		break;

	case Operation::CPY:
		cpy(cycles, memory, address, dummyAddress, hasPageCrossed);
		break;

	case Operation::DEX:
		dex(cycles);
		break;

	case Operation::DEY:
		dey(cycles);
		break;

	case Operation::DEC:
		dec(cycles, memory, address, dummyAddress, addrMode);
		break;

	case Operation::EOR:
		eor(cycles, memory, address, dummyAddress, hasPageCrossed);
		break;

	case Operation::INC:
		inc(cycles, memory, address, dummyAddress, addrMode);
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
		lda(cycles, memory, address, dummyAddress, hasPageCrossed);
		break;

	case Operation::LDX:
		ldx(cycles, memory, address, dummyAddress, hasPageCrossed);
		break;

	case Operation::LDY:
		ldy(cycles, memory, address, dummyAddress, hasPageCrossed);
		break;

	case Operation::LSR:
		lsr(cycles, memory, address, dummyAddress, addrMode);
		break;

	case Operation::NOP:
		nop(cycles);
		break;

	case Operation::ORA:
		ora(cycles, memory, address, dummyAddress, hasPageCrossed);
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
		rol(cycles, memory, address, dummyAddress, addrMode);
		break;

	case Operation::ROR:
		ror(cycles, memory, address, dummyAddress, addrMode);
		break;

	case Operation::RTI:
		rti(cycles, memory);
		break;

	case Operation::RTS:
		rts(cycles, memory);
		break;

	case Operation::SBC:
		sbc(cycles, memory, address, dummyAddress, hasPageCrossed);
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
		sta(cycles, memory, address, dummyAddress, addrMode);
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

void CPU::adc(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, bool hasPageCrossed)
{
	// Save previous Accumulator state, for status update
	u8 previousA = mA;

	// Read value from memory
	u8 memValue = readByte(cycles, memory, address);

	// Perform add with carry
	u16 newA = mA + memValue + mC;
	mA = newA & 0x00FF;

	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		readByte(cycles, memory, dummyAddress);
	
	// Update status flags
	adcUpdateStatus(newA, previousA, memValue);
}

void CPU::and_(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, bool hasPageCrossed)
{
	// Read value from memory
	u8 memValue = readByte(cycles, memory, address);

	// Perform '&' mask
	mA &= memValue;

	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		readByte(cycles, memory, dummyAddress);

	// Update status flags
	andUpdateStatus();
}

void CPU::asl(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, AddressingMode addrMode)
{
	// Read value from memory or accumulator
	u8 previousValue;
	if (addrMode == AddressingMode::Accumulator)
		previousValue = mA;
	else
		previousValue = readByte(cycles, memory, address);

	// Shift bytes
	u8 newValue = previousValue << 1;
	cycles--;

	// Write back value where it belongs
	if (addrMode == AddressingMode::Accumulator)
		mA = newValue;
	else
		writeByte(cycles, memory, address, newValue);

	// Dummy cycle on Abs.X writing
	if (addrMode == AddressingMode::AbsoluteX)
		readByte(cycles, memory, dummyAddress);

	// Update status flags
	aslUpdateStatus(previousValue, newValue);
}

void CPU::bcc(s32 &cycles, u16 address, bool hasPageCrossed)
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

void CPU::bcs(s32 &cycles, u16 address, bool hasPageCrossed)
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

void CPU::beq(s32 &cycles, u16 address, bool hasPageCrossed)
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

void CPU::bit(s32 &cycles, Memory &memory, u16 address)
{
	// Read value from memory
	u8 memValue = readByte(cycles, memory, address);

	// Bit tests
	mZ = (mA & memValue) == 0          ? 1 : 0;
	mV = (memValue & 0b0100'0000) != 0 ? 1 : 0;
	mN = (memValue & 0b1000'0000) != 0 ? 1 : 0;
}

void CPU::bmi(s32 &cycles, u16 address, bool hasPageCrossed)
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

void CPU::bne(s32 &cycles, u16 address, bool hasPageCrossed)
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

void CPU::bpl(s32 &cycles, u16 address, bool hasPageCrossed)
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

void CPU::brk(s32 &cycles, Memory& memory)
{
	// Dummy cycle (why would they do that...)
	mPc++;
	cycles--;

	// Push program counter & processor status
	u8 pcLsb = mPc & 0x00FF;
	u8 pcMsb = (mPc & 0xFF00) >> 8;
	mB = 1;
	u8 processorStatus = getProcessorStatus();
	stackPush(cycles, memory, pcMsb);
	stackPush(cycles, memory, pcLsb);
	stackPush(cycles, memory, processorStatus);
	mI = 1;
	
	// Fetch interrupt address
	u8 interruptAddressLsb = readByte(cycles, memory, IRQ_VECTOR_LSB); 
	u8 interruptAddressMsb = readByte(cycles, memory, IRQ_VECTOR_MSB); 

	// Program counter update
	mPc = ((u16)interruptAddressMsb << 8) | interruptAddressLsb;
}

void CPU::bvc(s32 &cycles, u16 address, bool hasPageCrossed)
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

void CPU::bvs(s32 &cycles, u16 address, bool hasPageCrossed)
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

void CPU::clc(s32 &cycles)
{
	// Update status
	mC = 0;
	cycles--;
}

void CPU::cld(s32 &cycles)
{
	// Update status
	mD = 0;
	cycles--;
}

void CPU::cli(s32 &cycles)
{
	// Update status
	mPreviousI = mI;
	mIsIDelayed = true;
	mI = 0;
	cycles--;
}

void CPU::clv(s32 &cycles)
{
	// Update status
	mV = 0;
	cycles--;
}

void CPU::cmp(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, bool hasPageCrossed)
{
	// Read value from memory
	u8 memValue = readByte(cycles, memory, address);

	// Perform substract to compare & update flags
	u16 comparisonValue = mA - memValue;
	mC = (comparisonValue & 0xFF00) == 0 ? 1 : 0;
	mZ = comparisonValue == 0            ? 1 : 0;
	mN = (comparisonValue & 0x0080) != 0 ? 1 : 0;

	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		readByte(cycles, memory, dummyAddress);
}

void CPU::cpx(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, bool hasPageCrossed)
{
	// Read value from memory
	u8 memValue = readByte(cycles, memory, address);

	// Perform substract to compare & update flags
	u16 comparisonValue = mX - memValue;
	mC = (comparisonValue & 0xFF00) == 0 ? 1 : 0;
	mZ = comparisonValue == 0            ? 1 : 0;
	mN = (comparisonValue & 0x0080) != 0 ? 1 : 0;

	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		readByte(cycles, memory, dummyAddress);
}

void CPU::cpy(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, bool hasPageCrossed)
{
	// Read value from memory
	u8 memValue = readByte(cycles, memory, address);

	// Perform substract to compare & update flags
	u16 comparisonValue = mY - memValue;
	mC = (comparisonValue & 0xFF00) == 0 ? 1 : 0;
	mZ = comparisonValue == 0            ? 1 : 0;
	mN = (comparisonValue & 0x0080) != 0 ? 1 : 0;

	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		readByte(cycles, memory, dummyAddress);
}

void CPU::dec(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, AddressingMode addrMode)
{
	// Read value from memory
	u8 memValue = readByte(cycles, memory, address);

	// Increment value
	memValue--;
	cycles--;
	
	// Write value to memory
	writeByte(cycles, memory, address, memValue);

	// Dummy cycle for Abs.X
	if (addrMode == AddressingMode::AbsoluteX)
		readByte(cycles, memory, dummyAddress);

	// Update status flags
	decUpdateStatus(memValue);
}

void CPU::dex(s32 &cycles)
{
	// Decrement X register
	mX--;
	cycles--;

	// Update status flags
	dexUpdateStatus();
}

void CPU::dey(s32 &cycles)
{
	// Decrement Y register
	mY--;
	cycles--;

	// Update status flags
	deyUpdateStatus();
}

void CPU::eor(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, bool hasPageCrossed)
{
	// Read value from memory
	u8 memValue = readByte(cycles, memory, address);

	// Perform '^' mask
	mA ^= memValue;

	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		readByte(cycles, memory, dummyAddress);

	// Update status flags
	eorUpdateStatus();
}

void CPU::inc(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, AddressingMode addrMode)
{
	// Read value from memory
	u8 memValue = readByte(cycles, memory, address);

	// Increment value
	memValue++;
	cycles--;
	
	// Write value to memory
	writeByte(cycles, memory, address, memValue);

	// Dummy cycle for Abs.X
	if (addrMode == AddressingMode::AbsoluteX)
		readByte(cycles, memory, dummyAddress);

	// Update status flags
	incUpdateStatus(memValue);
}

void CPU::inx(s32 &cycles)
{
	// Increment X register
	mX++;
	cycles--;

	// Update status flags
	inxUpdateStatus();
}

void CPU::iny(s32 &cycles)
{
	// Increment Y register
	mY++;
	cycles--;

	// Update status flags
	inyUpdateStatus();
}

void CPU::jmp(u16 address)
{
	// Jump to location
	mPc = address;
}

void CPU::jsr(s32 &cycles, Memory &memory, u16 subroutineAddress)
{
	// Push Program counter -1 (return address) to stack (2 cycles)
	u16 returnAddress = mPc - 1;
	u8 returnAddressLsb = (returnAddress & 0x00FF);
	u8 returnAddressMsb = (returnAddress & 0xFF00) >> 8;
	stackPush(cycles, memory, returnAddressMsb);
	stackPush(cycles, memory, returnAddressLsb);

	// Set Program counter (1 cycle)
	mPc = subroutineAddress;
	cycles--;
}

void CPU::lda(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, bool hasPageCrossed)
{
	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		readByte(cycles, memory, dummyAddress);

	// Read memory
	mA = readByte(cycles, memory, address);
	
	// Update status flags
	ldaUpdateStatus();
}

void CPU::ldx(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, bool hasPageCrossed)
{
	// Read memory
	mX = readByte(cycles, memory, address);
	
	// Decrement cycles count if page crossing (abs.Y)
	if (hasPageCrossed)
		readByte(cycles, memory, dummyAddress);

	// Update status flags
	ldxUpdateStatus();
}

void CPU::ldy(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, bool hasPageCrossed)
{
	// Read memory
	mY = readByte(cycles, memory, address);
	
	// Decrement cycles count if page crossing (abs.Y)
	if (hasPageCrossed)
		readByte(cycles, memory, dummyAddress);

	// Update status flags
	ldyUpdateStatus();
}

void CPU::lsr(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, AddressingMode addrMode)
{
	// Read value from memory or accumulator
	u8 previousValue;
	if (addrMode == AddressingMode::Accumulator)
		previousValue = mA;
	else
		previousValue = readByte(cycles, memory, address);

	// Shift bytes
	u8 newValue = previousValue >> 1;
	cycles--;

	// Write back value where it belongs
	if (addrMode == AddressingMode::Accumulator)
		mA = newValue;
	else
		writeByte(cycles, memory, address, newValue);

	// Dummy cycle on Abs.X writing
	if (addrMode == AddressingMode::AbsoluteX)
		readByte(cycles, memory, dummyAddress);

	// Update status flags
	lsrUpdateStatus(previousValue, newValue);
}

void CPU::nop(s32 &cycles)
{
	// Do nothing but a dummy cycle
	cycles--;
}

void CPU::ora(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, bool hasPageCrossed)
{
	// Read value from memory
	u8 memValue = readByte(cycles, memory, address);

	// Perform '|' mask
	mA |= memValue;

	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		readByte(cycles, memory, dummyAddress);

	// Update status flags
	oraUpdateStatus();
}

void CPU::pha(s32 &cycles, Memory& memory)
{
	// Push A to Stack (simulate A read with a clock cycle)
	cycles--;
	stackPush(cycles, memory, mA);
}

void CPU::php(s32 &cycles, Memory &memory)
{
	// Push processor status to stack (simulate PStatus read with a clock cycle)
	u8 status = getProcessorStatus();
	cycles--;

	stackPush(cycles, memory, status);
}

void CPU::pla(s32 &cycles, Memory &memory)
{
	// Pull A (+ 1 dead cycle, can't figure out why...)
	mA = stackPull(cycles, memory);
	cycles--;

	// Update status flags
	plaUpdateStatus();
}

void CPU::plp(s32 &cycles, Memory &memory)
{
	// Pull Processor status (+ 1 dead cycle, can't figure out why...)
	u8 processorStatus = stackPull(cycles, memory);
	cycles--;

	// Update status flags
	mPreviousI = mI;
	mIsIDelayed = true;
	setProcessorStatus(processorStatus);
}

void CPU::rol(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, AddressingMode addrMode)
{
	// Read value from memory or accumulator
	u8 previousValue;
	if (addrMode == AddressingMode::Accumulator)
		previousValue = mA;
	else
		previousValue = readByte(cycles, memory, address);

	// Shift bytes
	u8 newValue = (previousValue << 1) | mC;
	cycles--;

	// Dummy cycle on Abs.X writing
	if (addrMode == AddressingMode::AbsoluteX)
		readByte(cycles, memory, dummyAddress);

	// Write back value where it belongs
	if (addrMode == AddressingMode::Accumulator)
		mA = newValue;
	else
		writeByte(cycles, memory, address, newValue);

	// Update status flags
	rolUpdateStatus(previousValue, newValue);
}

void CPU::ror(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, AddressingMode addrMode)
{
	// Read value from memory or accumulator
	u8 previousValue;
	if (addrMode == AddressingMode::Accumulator)
		previousValue = mA;
	else
		previousValue = readByte(cycles, memory, address);

	// Shift bytes
	u8 newValue = (previousValue >> 1) | (mC << 7);
	cycles--;

	// Write back value where it belongs
	if (addrMode == AddressingMode::Accumulator)
		mA = newValue;
	else
		writeByte(cycles, memory, address, newValue);

	// Dummy cycle on Abs.X writing
	if (addrMode == AddressingMode::AbsoluteX)
		readByte(cycles, memory, dummyAddress);

	// Update status flags
	rorUpdateStatus(previousValue, newValue);
}

void CPU::rti(s32 &cycles, Memory &memory)
{
	// Pull Processor status & Program counter (return address) to stack (4 cycles)
	cycles++; // Simulate stack pull internal optimisation
	u8 processorStatus = stackPull(cycles, memory);
	setProcessorStatus(processorStatus);

	u8 returnAddressLsb = stackPull(cycles, memory);
	u8 returnAddressMsb = stackPull(cycles, memory);
	u16 returnAddress = ((u16)returnAddressMsb << 8) | returnAddressLsb;

	// Set Program counter (1 cycle)
	mPc = returnAddress;
}

void CPU::rts(s32 &cycles, Memory &memory)
{
	// Pull Program counter -1 (return address) to stack (4 cycles)
	u8 returnAddressLsb = stackPull(cycles, memory);
	u8 returnAddressMsb = stackPull(cycles, memory);
	u16 returnAddress = ((u16)returnAddressMsb << 8) | returnAddressLsb;

	// Set Program counter + 1 (1 cycle)
	mPc = returnAddress + 1;
	cycles--;
}

void CPU::sbc(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, bool hasPageCrossed)
{
	// Save previous Accumulator state, for status update
	u8 previousA = mA;

	// Read value from memory
	u8 memValue = readByte(cycles, memory, address);

	// Perform substract with carry
	u16 newA = mA - memValue - ((u16)1 - mC);
	mA = newA & 0x00FF;

	// Decrement cycles count if page crossing (abs.X, abs.Y, ind.Y)
	if (hasPageCrossed)
		readByte(cycles, memory, dummyAddress);
	
	// Update status flags
	sbcUpdateStatus(newA, previousA, memValue);
}

void CPU::sec(s32 &cycles)
{
	// Update status
	mC = 1;
	cycles--;
}

void CPU::sed(s32 &cycles)
{
	// Update status
	mD = 1;
	cycles--;
}

void CPU::sei(s32 &cycles)
{
	// Update status
	mPreviousI = mI;
	mIsIDelayed = true;
	mI = 1;
	cycles--;
}

void CPU::sta(s32 &cycles, Memory &memory, u16 address, u16 dummyAddress, AddressingMode addrMode)
{
	// A -> mem[addr]
	writeByte(cycles, memory, address, mA);	

	// Dummy cycle for (abs.X, abs.Y, ind.Y)
	if (addrMode == AddressingMode::AbsoluteX ||
		addrMode == AddressingMode::AbsoluteY ||
		addrMode == AddressingMode::IndirectY)
		readByte(cycles, memory, dummyAddress);
}

void CPU::stx(s32 &cycles, Memory &memory, u16 address)
{
	// X -> mem[addr]
	writeByte(cycles, memory, address, mX);	
}

void CPU::sty(s32 &cycles, Memory &memory, u16 address)
{
	// Y -> mem[addr]
	writeByte(cycles, memory, address, mY);	
}

void CPU::tax(s32 &cycles)
{
	// A -> X
	mX = mA;
	cycles--;

	// Update status flags
	taxUpdateStatus();
}

void CPU::tay(s32 &cycles)
{
	// A -> Y
	mY = mA;
	cycles--;

	// Update status flags
	tayUpdateStatus();
}

void CPU::tsx(s32 &cycles)
{
	// SP -> X
	mX = mSp;
	cycles--;

	// Update status flags
	tsxUpdateStatus();
}

void CPU::txa(s32 &cycles)
{
	// X -> A
	mA = mX;
	cycles--;

	// Update status flags
	txaUpdateStatus();
}

void CPU::txs(s32 &cycles)
{
	// X -> SP
	mSp = mX;
	cycles--;
}

void CPU::tya(s32 &cycles)
{
	// Y -> A
	mA = mY;
	cycles--;

	// Update status flags
	tyaUpdateStatus();
}

u8 CPU::getProcessorStatus() const
{
	u8 processorStatus = 0x00;
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

void CPU::setProcessorStatus(u8 processorStatus)
{
	mC = (processorStatus >> 0) & 0x01;
	mZ = (processorStatus >> 1) & 0x01;
	mI = (processorStatus >> 2) & 0x01;
	mD = (processorStatus >> 3) & 0x01;
	mV = (processorStatus >> 6) & 0x01;
	mN = (processorStatus >> 7) & 0x01;
}

void CPU::adcUpdateStatus(u16 newA, u8 operandA, u8 operandM)
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

void CPU::aslUpdateStatus(u8 previousValue, u8 newValue)
{
	mC = (previousValue & 0x80) != 0 ? 1 : 0;
	mZ = newValue == 0               ? 1 : 0;
	mN = (newValue & 0x80) != 0      ? 1 : 0;
}

void CPU::decUpdateStatus(u8 memValue)
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

void CPU::incUpdateStatus(u8 memValue)
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

void CPU::lsrUpdateStatus(u8 previousValue, u8 newValue)
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

void CPU::rolUpdateStatus(u8 previousValue, u8 newValue)
{
	mC = (previousValue & 0x80) != 0 ? 1 : 0;
	mZ = newValue == 0               ? 1 : 0;
	mN = (newValue & 0x80) != 0      ? 1 : 0;
}

void CPU::rorUpdateStatus(u8 previousValue, u8 newValue)
{
	mC = (previousValue & 0x01) != 0 ? 1 : 0;
	mZ = newValue == 0               ? 1 : 0;
	mN = (newValue & 0x80) != 0      ? 1 : 0;
}

void CPU::sbcUpdateStatus(u16 newA, u8 operandA, u8 operandM)
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

void CPU::cpuLog()
{
	if (!gIsTraceLogCpuEnabled)
		return;

	std::stringstream cpuTraceStream;
	cpuTraceStream << "PC:$" << std::uppercase << std::hex << mPc 
	               << ", SP:$" << std::setw(2) << +mSp  
				   << ", A:$" << std::setw(2) << +mA  
				   << ", X:$" << std::setw(2) << +mX  
				   << ", Y:$" << std::setw(2) << +mY 
				   << ", P:$" << std::setw(2) << +getProcessorStatus();
	traceLog(cpuTraceStream.str());
}

void CPU::cpuLogIrq()
{
	if (!gIsTraceLogCpuEnabled)
		return;

	std::stringstream cpuTraceStream;
	cpuTraceStream << "IRQ at PC:$" << std::uppercase << std::hex << mPc 
	               << ", SP:$" << std::setw(2) << +mSp  
				   << ", A:$" << std::setw(2) << +mA  
				   << ", X:$" << std::setw(2) << +mX  
				   << ", Y:$" << std::setw(2) << +mY 
				   << ", P:$" << std::setw(2) << +getProcessorStatus();
	traceLog(cpuTraceStream.str());
}

void CPU::cpuLogNmi()
{
	if (!gIsTraceLogCpuEnabled)
		return;

	std::stringstream cpuTraceStream;
	cpuTraceStream << "NMI at PC:$" << std::uppercase << std::hex << mPc 
	               << ", SP:$" << std::setw(2) << +mSp  
				   << ", A:$" << std::setw(2) << +mA  
				   << ", X:$" << std::setw(2) << +mX  
				   << ", Y:$" << std::setw(2) << +mY 
				   << ", P:$" << std::setw(2) << +getProcessorStatus();
	traceLog(cpuTraceStream.str());
}

void CPU::cpuLogDisassembly(const char *disassembly)
{
	if (!gIsTraceLogCpuEnabled)
		return;
	
	traceLog(std::string(" ") + disassembly);
}
void CPU::cpuLogEnd()
{
	if (!gIsTraceLogCpuEnabled)
		return;

	traceLog("\n");
}
