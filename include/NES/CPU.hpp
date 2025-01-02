#pragma once

#include "NES/Config.hpp"
#include "NES/Memory.hpp"
#include "NES/CPUConstants.hpp"

class CPU
{
public:
    // *********** External calls *********** //
    s32 reset(Memory& memory);
    s32 irq(Memory& memory);
    s32 nmi(Memory& memory);
    s32 execute(s32 cycles, Memory& memory);

    // ******** Accessors ******** //
    // Getters
    inline u16 getPc() const { return mPc; }
    inline u8 getSp() const { return mSp; }

    inline u8 getA() const { return mA; }
    inline u8 getX() const { return mX; }
    inline u8 getY() const { return mY; }

    inline u8 getC() const { return mC; }
    inline u8 getZ() const { return mZ; }
    inline u8 getI() const { return mI; }
    inline u8 getD() const { return mD; }
    inline u8 getB() const { return mB; }
    inline u8 getV() const { return mV; }
    inline u8 getN() const { return mN; }
    
#ifdef TEST_6502
    // Setters
    inline void setPc(word value) { mPc = value; }
    inline void setSp(byte value) { mSp = value; }

    inline void setA(byte value) { mA = value; }
    inline void setX(byte value) { mX = value; }
    inline void setY(byte value) { mY = value; }

    inline void setC(byte value) { mC = value; }
    inline void setZ(byte value) { mZ = value; }
    inline void setI(byte value) { mI = value; }
    inline void setD(byte value) { mD = value; }
    inline void setB(byte value) { mB = value; }
    inline void setV(byte value) { mV = value; }
    inline void setN(byte value) { mN = value; }
#endif

private:
    // ******** Internal behaviour ******** //
    // Read/Write memory
    u8 fetchByte(s32& cycles, Memory& memory);
    u16 fetchWord(s32& cycles, Memory& memory);
    u8 readByte(s32& cycles, Memory& memory, u16 address);
    void writeByte(s32& cycles, Memory& memory, u16 address, u8 value);
    void stackPush(s32& cycles, Memory& memory, u8 value);
    u8 stackPull(s32& cycles, Memory& memory);

    // *** Instruction execution *** //
    u16 fetchAddr(s32& cycles, Memory& memory, AddressingMode addrMode, u16& dummyAddress, bool& hasPageCrossed);
    void executeInstruction(s32& cycles, Memory& memory, instruction_t instruction, u16 address, u16 dummyAddress, bool hasPageCrossed);
    
    // *** Instructions *** //
    void adc(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, bool hasPageCrossed);
    void and_(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, bool hasPageCrossed); // "and" is a C++ keyword... it ruins the string alignment...
    void asl(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, AddressingMode addrMode);
    void bcc(s32& cycles, u16 address, bool hasPageCrossed);
    void bcs(s32& cycles, u16 address, bool hasPageCrossed);
    void beq(s32& cycles, u16 address, bool hasPageCrossed);
    void bit(s32& cycles, Memory& memory, u16 address);
    void bmi(s32& cycles, u16 address, bool hasPageCrossed);
    void bne(s32& cycles, u16 address, bool hasPageCrossed);
    void bpl(s32& cycles, u16 address, bool hasPageCrossed);
    void brk(s32& cycles, Memory& memory);
    void bvc(s32& cycles, u16 address, bool hasPageCrossed);
    void bvs(s32& cycles, u16 address, bool hasPageCrossed);
    void clc(s32& cycles);
    void cld(s32& cycles);
    void cli(s32& cycles);
    void clv(s32& cycles);
    void cmp(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, bool hasPageCrossed);
    void cpx(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, bool hasPageCrossed);
    void cpy(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, bool hasPageCrossed);
    void dec(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, AddressingMode addrMode);
    void dex(s32& cycles);
    void dey(s32& cycles);
    void eor(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, bool hasPageCrossed);
    void inc(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, AddressingMode addrMode);
    void inx(s32& cycles);
    void iny(s32& cycles);
    void jmp(u16 address);
    void jsr(s32& cycles, Memory& memory, u16 subroutineAddress);
    void lda(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, bool hasPageCrossed);
    void ldx(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, bool hasPageCrossed);
    void ldy(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, bool hasPageCrossed);
    void lsr(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, AddressingMode addrMode);
    void nop(s32& cycles);
    void ora(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, bool hasPageCrossed);
    void pha(s32& cycles, Memory& memory);
    void php(s32& cycles, Memory& memory);
    void pla(s32& cycles, Memory& memory);
    void plp(s32& cycles, Memory& memory);
    void rol(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, AddressingMode addrMode);
    void ror(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, AddressingMode addrMode);
    void rts(s32& cycles, Memory& memory);
    void rti(s32& cycles, Memory& memory);
    void sbc(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, bool hasPageCrossed);
    void sec(s32& cycles);
    void sed(s32& cycles);
    void sei(s32& cycles);
    void sta(s32& cycles, Memory& memory, u16 address, u16 dummyAddress, AddressingMode addrMode);
    void stx(s32& cycles, Memory& memory, u16 address);
    void sty(s32& cycles, Memory& memory, u16 address);
    void tax(s32& cycles);
    void tay(s32& cycles);
    void tsx(s32& cycles);
    void txa(s32& cycles);
    void txs(s32& cycles);
    void tya(s32& cycles);

    // *** Status flags *** //
    u8 getProcessorStatus() const;
    void setProcessorStatus(u8 processorStatus);
    void setProcessorStatusIDelayed(u8 processorStatus);
    
    void adcUpdateStatus(u16 newA, u8 operandA, u8 operandM);
    void andUpdateStatus();
    void aslUpdateStatus(u8 previousValue, u8 newValue);
    void decUpdateStatus(u8 memValue);
    void dexUpdateStatus();
    void deyUpdateStatus();
    void eorUpdateStatus();
    void incUpdateStatus(u8 memValue);
    void inxUpdateStatus();
    void inyUpdateStatus();
    void ldaUpdateStatus();
    void ldxUpdateStatus();
    void ldyUpdateStatus();
    void lsrUpdateStatus(u8 previousValue, u8 newValue);
    void oraUpdateStatus();
    void plaUpdateStatus();
    void rolUpdateStatus(u8 previousValue, u8 newValue);
    void rorUpdateStatus(u8 previousValue, u8 newValue);
    void sbcUpdateStatus(u16 newA, u8 operandA, u8 operandM);
    void taxUpdateStatus();
    void tayUpdateStatus();
    void tsxUpdateStatus();
    void txaUpdateStatus();
    void tyaUpdateStatus();

    void cpuLog();
    void cpuLogIrq();
    void cpuLogNmi();
    void cpuLogDisassembly(const char* disassembly);
    void cpuLogEnd();

    // ********** Registers    ********** //
    u16 mPc;       // Program Counter
    u8 mSp;       // Stack Pointer

    u8 mA;        // Accumulator
    u8 mX;        // X
    u8 mY;        // Y

    // Processor status flags
    u8 mC : 1;    // Carry
    u8 mZ : 1;    // Zero
    u8 mI : 1;    // Interrupt disable
    u8 mD : 1;    // Decimal
    u8 mB : 1;    // Break
    u8 mU : 1;    // Unused
    u8 mV : 1;    // Overflow
    u8 mN : 1;    // Negative

    u8 mPreviousI: 1; // To emulate the delay on CLI SEI PLP
    bool mIsIDelayed;
};
