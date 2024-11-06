#pragma once

#include <cstdint>
#include <array>

#include "NES/Config.hpp"
#include "NES/Memory.hpp"

constexpr u32 PPU_OUTPUT_WIDTH = 256;
constexpr u32 PPU_OUTPUT_HEIGHT = 240;
constexpr u32 PPU_OUTPUT_CHANNELS = 3;

using picture_t = std::array<std::array<std::array<u8, PPU_OUTPUT_CHANNELS>, PPU_OUTPUT_WIDTH>, PPU_OUTPUT_HEIGHT>;

class PPU
{
public:
    void reset();
    void executeOneCycle(Memory& memory);

    void writeRegister(Memory& memory, u16 address, u8 value);
    u8 readRegister(Memory& memory, u16 address);

    inline const picture_t& getPicture() const { return mPicture; }
    inline bool getVBlankNMISignal() const { return mVBlankNMISignal; }
    inline bool isImageReady() const { return mIsImageReady; }
    inline void clearIsImageReady() { mIsImageReady = false; }
    inline void clearNMISignal() { mVBlankNMISignal = false; mNMICanOccur = false; }

private:
    u8 readByte(Memory& memory, u16 address);
    void writeByte(Memory& memory, u16 address, u8 value);

    void executeVisibleScanline(Memory& memory);
    void executePostRenderScanline(Memory& memory);
    void executeVBlankScanline(Memory& memory);
    void executePreRenderScanline(Memory& memory);

    void setPictureColor(u8 colorCode, u32 row, u32 col);

    void incrementCoarseX();
    void incrementY();

    void loadX();
    void loadY();

    // Rendering
    picture_t mPicture;
    bool mIsImageReady;
    u16 mScanlineCount;
    u16 mCycleCount;

    struct backgroundData
    {
        u8 nt;
        u8 at;
        u8 ptLsb;
        u8 ptMsb;
    } mBgData;

    bool mIsFirstPrerenderPassed;

    // Registers
    static constexpr u16 PPUCTRL_CPU_ADDR   = 0x2000;
    static constexpr u16 PPUMASK_CPU_ADDR   = 0x2001;
    static constexpr u16 PPUSTATUS_CPU_ADDR = 0x2002;
    static constexpr u16 OAMADDR_CPU_ADDR   = 0x2003;
    static constexpr u16 OAMDATA_CPU_ADDR   = 0x2004;
    static constexpr u16 PPUSCROLL_CPU_ADDR = 0x2005;
    static constexpr u16 PPUADDR_CPU_ADDR   = 0x2006;
    static constexpr u16 PPUDATA_CPU_ADDR   = 0x2007;

    u8 mPpuCtrl;
    u8 mPpuMask;
    u8 mPpuStatus;
    u8 mOamAddr;
    u8 mOamData;
    u8 mPpuScrollX;
    u8 mPpuScrollY;
    u16 mPpuAddr;
    u8 mPpuData;

    u16 mV : 15;
    u16 mT : 15;
    u8 mX : 3;
    u8 mW : 1;

    bool mIsOddFrame;

    // OAM
    std::array<u8, 256> mOam;
    std::array<u8, 32> mOamSecondary;

    // Palette RAM
    std::array<u8, 32> mPaletteRam;

    // NMI
    bool mVBlankNMISignal;
    bool mNMICanOccur;

    // Color Lookup table
    static constexpr std::array<std::array<u8, 3>, 64> LUT = 
    {{
        { 124,124,124 },
        { 0,0,252 },
        { 0,0,188 },
        { 68,40,188 },
        { 148,0,132 },
        { 168,0,32 },
        { 168,16,0 },
        { 136,20,0 },
        { 80,48,0 },
        { 0,120,0 },
        { 0,104,0 },
        { 0,88,0 },
        { 0,64,88 },
        { 0,0,0 },
        { 0,0,0 },
        { 0,0,0 },
        { 188,188,188 },
        { 0,120,248 },
        { 0,88,248 },
        { 104,68,252 },
        { 216,0,204 },
        { 228,0,88 },
        { 248,56,0 },
        { 228,92,16 },
        { 172,124,0 },
        { 0,184,0 },
        { 0,168,0 },
        { 0,168,68 },
        { 0,136,136 },
        { 0,0,0 },
        { 0,0,0 },
        { 0,0,0 },
        { 248,248,248 },
        { 60,188,252 },
        { 104,136,252 },
        { 152,120,248 },
        { 248,120,248 },
        { 248,88,152 },
        { 248,120,88 },
        { 252,160,68 },
        { 248,184,0 },
        { 184,248,24 },
        { 88,216,84 },
        { 88,248,152 },
        { 0,232,216 },
        { 120,120,120 },
        { 0,0,0 },
        { 0,0,0 },
        { 252,252,252 },
        { 164,228,252 },
        { 184,184,248 },
        { 216,184,248 },
        { 248,184,248 },
        { 248,164,192 },
        { 240,208,176 },
        { 252,224,168 },
        { 248,216,120 },
        { 216,248,120 },
        { 184,248,184 },
        { 184,248,216 },
        { 0,252,252 },
        { 248,216,248 },
        { 0,0,0 },
        { 0,0,0 }
    }};
};
