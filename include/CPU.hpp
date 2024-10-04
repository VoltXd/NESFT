#pragma once

#include "Config.hpp"
#include "Memory.hpp"
#include "CPUConstants.hpp"

class CPU
{
public:
    // *********** External calls *********** //
    void reset(Memory& memory);
    sdword execute(sdword cycles, Memory& memory);

#ifdef TEST
    // ******** Accessors ******** //
    // Getters
    inline word getPc() const { return mPc; }
    inline byte getSp() const { return mSp; }

    inline byte getA() const { return mA; }
    inline byte getX() const { return mX; }
    inline byte getY() const { return mY; }

    inline byte getC() const { return mC; }
    inline byte getZ() const { return mZ; }
    inline byte getI() const { return mI; }
    inline byte getD() const { return mD; }
    inline byte getB() const { return mB; }
    inline byte getV() const { return mV; }
    inline byte getN() const { return mN; }
    
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
    byte fetchByte(sdword& cycles, Memory& memory);
    word fetchWord(sdword& cycles, Memory& memory);
    byte readByte(sdword& cycles, Memory& memory, word address);
    void writeByte(sdword& cycles, Memory& memory, word address, byte value);
    void stackPush(sdword& cycles, Memory& memory, byte value);
    byte stackPull(sdword& cycles, Memory& memory);

    // *** Instruction execution *** //
    word fetchAddr(sdword& cycles, Memory& memory, AddressingMode addrMode, bool& hasPageCrossed);
    void executeInstruction(sdword& cycles, Memory& memory, instruction_t instruction, word address, bool hasPageCrossed);
    
    // *** Instructions *** //
    void adc(sdword& cycles, Memory& memory, word address, bool hasPageCrossed);
    void and_(sdword& cycles, Memory& memory, word address, bool hasPageCrossed); // "and" is a C++ keyword... it ruins the string alignment...
    void asl(sdword& cycles, Memory& memory, word address, AddressingMode addrMode);
    void bcc(sdword& cycles, word address, bool hasPageCrossed);
    void bcs(sdword& cycles, word address, bool hasPageCrossed);
    void beq(sdword& cycles, word address, bool hasPageCrossed);
    void bit(sdword& cycles, Memory& memory, word address);
    void bmi(sdword& cycles, word address, bool hasPageCrossed);
    void bne(sdword& cycles, word address, bool hasPageCrossed);
    void bpl(sdword& cycles, word address, bool hasPageCrossed);
    void brk(sdword& cycles, Memory& memory);
    void bvc(sdword& cycles, word address, bool hasPageCrossed);
    void bvs(sdword& cycles, word address, bool hasPageCrossed);
    void clc(sdword& cycles);
    void cld(sdword& cycles);
    void cli(sdword& cycles);
    void clv(sdword& cycles);
    void cmp(sdword& cycles, Memory& memory, word address, bool hasPageCrossed);
    void cpx(sdword& cycles, Memory& memory, word address, bool hasPageCrossed);
    void cpy(sdword& cycles, Memory& memory, word address, bool hasPageCrossed);
    void dec(sdword& cycles, Memory& memory, word address, AddressingMode addrMode);
    void dex(sdword& cycles);
    void dey(sdword& cycles);
    void eor(sdword& cycles, Memory& memory, word address, bool hasPageCrossed);
    void inc(sdword& cycles, Memory& memory, word address, AddressingMode addrMode);
    void inx(sdword& cycles);
    void iny(sdword& cycles);
    void jmp(word address);
    void jsr(sdword& cycles, Memory& memory, word subroutineAddress);
    void lda(sdword& cycles, Memory& memory, word address, bool hasPageCrossed);
    void ldx(sdword& cycles, Memory& memory, word address, bool hasPageCrossed);
    void ldy(sdword& cycles, Memory& memory, word address, bool hasPageCrossed);
    void lsr(sdword& cycles, Memory& memory, word address, AddressingMode addrMode);
    void nop(sdword& cycles);
    void ora(sdword& cycles, Memory& memory, word address, bool hasPageCrossed);
    void pha(sdword& cycles, Memory& memory);
    void php(sdword& cycles, Memory& memory);
    void pla(sdword& cycles, Memory& memory);
    void plp(sdword& cycles, Memory& memory);
    void rol(sdword& cycles, Memory& memory, word address, AddressingMode addrMode);
    void ror(sdword& cycles, Memory& memory, word address, AddressingMode addrMode);
    void rts(sdword& cycles, Memory& memory);
    void rti(sdword& cycles, Memory& memory);
    void sbc(sdword& cycles, Memory& memory, word address, bool hasPageCrossed);
    void sec(sdword& cycles);
    void sed(sdword& cycles);
    void sei(sdword& cycles);
    void sta(sdword& cycles, Memory& memory, word address, AddressingMode addrMode);
    void stx(sdword& cycles, Memory& memory, word address);
    void sty(sdword& cycles, Memory& memory, word address);
    void tax(sdword& cycles);
    void tay(sdword& cycles);
    void tsx(sdword& cycles);
    void txa(sdword& cycles);
    void txs(sdword& cycles);
    void tya(sdword& cycles);

    // *** Status flags *** //
    byte getProcessorStatus() const;
    void setProcessorStatus(byte processorStatus);
    
    void adcUpdateStatus(word newA, byte operandA, byte operandM);
    void andUpdateStatus();
    void aslUpdateStatus(byte previousValue, byte newValue);
    void decUpdateStatus(byte memValue);
    void dexUpdateStatus();
    void deyUpdateStatus();
    void eorUpdateStatus();
    void incUpdateStatus(byte memValue);
    void inxUpdateStatus();
    void inyUpdateStatus();
    void ldaUpdateStatus();
    void ldxUpdateStatus();
    void ldyUpdateStatus();
    void lsrUpdateStatus(byte previousValue, byte newValue);
    void oraUpdateStatus();
    void plaUpdateStatus();
    void rolUpdateStatus(byte previousValue, byte newValue);
    void rorUpdateStatus(byte previousValue, byte newValue);
    void sbcUpdateStatus(word newA, byte operandA, byte operandM);
    void taxUpdateStatus();
    void tayUpdateStatus();
    void tsxUpdateStatus();
    void txaUpdateStatus();
    void tyaUpdateStatus();

    // ********** Registers    ********** //
    word mPc;       // Program Counter
    byte mSp;       // Stack Pointer

    byte mA;        // Accumulator
    byte mX;        // X
    byte mY;        // Y

    // Processor status flags
    byte mC : 1;    // Carry
    byte mZ : 1;    // Zero
    byte mI : 1;    // Interrupt disable
    byte mD : 1;    // Decimal
    byte mB : 1;    // Break
    byte mU : 1;    // Unused
    byte mV : 1;    // Overflow
    byte mN : 1;    // Negative
};
