#pragma once

#include "Config.hpp"
#include "Memory.hpp"
#include "RegisterConstants.hpp"

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
    // Read memory
    byte fetchByte(sdword& cycles, Memory& memory);
    word fetchWord(sdword& cycles, Memory& memory);
    byte readByte(sdword& cycles, Memory& memory, word address);
    void stackPush(sdword& cycles, Memory& memory, byte value);

    // *** Addressing modes *** //
    // Implicit & Accumulator modes does not need implementation...
    byte fetchImmediate(sdword& cycles, Memory& memory);
    word fetchAddrZeroPage(sdword& cycles, Memory& memory);
    word fetchAddrZeroPageX(sdword& cycles, Memory& memory);
    word fetchAddrZeroPageY(sdword& cycles, Memory& memory);
    word fetchAddrRelative(sdword& cycles, Memory& memory);
    word fetchAddrAbsolute(sdword& cycles, Memory& memory);
    word fetchAddrAbsoluteX(sdword& cycles, Memory& memory);
    word fetchAddrAbsoluteY(sdword& cycles, Memory& memory);
    word fetchAddrIndirect(sdword& cycles, Memory& memory);
    word fetchAddrIndirectX(sdword& cycles, Memory& memory);
    word fetchAddrIndirectY(sdword& cycles, Memory& memory);
    
    // *** Instructions *** //
    // JSR
    void jsr(sdword& cycles, Memory& memory);

    // LDA
    void ldaImm(sdword& cycles, Memory& memory);
    void ldaZp(sdword& cycles, Memory& memory);
    void ldaZpX(sdword& cycles, Memory& memory);
    void ldaAbs(sdword& cycles, Memory& memory);
    void ldaAbsX(sdword& cycles, Memory& memory);
    void ldaAbsY(sdword& cycles, Memory& memory);
    void ldaIndX(sdword& cycles, Memory& memory);
    void ldaIndY(sdword& cycles, Memory& memory);
    void ldaUpdateStatus();

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
    byte mV : 1;    // Overflow
    byte mN : 1;    // Negative
};
