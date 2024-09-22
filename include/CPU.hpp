#pragma once

#include "Config.hpp"
#include "Memory.hpp"
#include "RegisterConstants.hpp"

class CPU
{
public:
    // *********** External calls *********** //
    void reset(Memory& memory);
    void execute(dword cycles, Memory& memory);

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

private:
    // ******** Internal behaviour ******** //
    // Read memory
    byte fetchByte(dword& cycles, Memory& memory);
    word fetchWord(dword& cycles, Memory& memory);
    byte readByte(dword& cycles, Memory& memory, dword address);

    // Instructions
    void jsr(dword& cycles, Memory& memory);
    void ldaImm(dword& cycles, Memory& memory);
    void ldaZp(dword& cycles, Memory& memory);
    void ldaZpX(dword& cycles, Memory& memory);
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
