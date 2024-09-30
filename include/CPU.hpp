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
    // Read memory
    byte fetchByte(sdword& cycles, Memory& memory);
    word fetchWord(sdword& cycles, Memory& memory);
    byte readByte(sdword& cycles, Memory& memory, word address);
    void stackPush(sdword& cycles, Memory& memory, byte value);

    // *** Instruction execution *** //
    word fetchAddr(sdword& cycles, Memory& memory, AddressingMode addrMode, bool& hasPageCrossed);
    void executeInstruction(sdword& cycles, Memory& memory, Operation operation, word address, bool hasPageCrossed);
    
    // *** Instructions *** //
    void jsr(sdword& cycles, Memory& memory, word subroutineAddress);
    void lda(sdword& cycles, Memory& memory, word address);

    // *** Update status flags *** //
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
