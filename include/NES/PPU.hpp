#pragma once

#include <cstdint>
#include <array>

#include "NES/Config.hpp"
#include "NES/Memory.hpp"

constexpr u32 PPU_OUTPUT_WIDTH = 256 + 8;
constexpr u32 PPU_OUTPUT_HEIGHT = 240;
constexpr u32 PPU_OUTPUT_CHANNELS = 3;

using picture_t = std::array<std::array<std::array<u8, PPU_OUTPUT_CHANNELS>, PPU_OUTPUT_WIDTH>, PPU_OUTPUT_HEIGHT>;

class PPU
{
public:
    void reset();
    void executeOneCycle(Memory& memory);

    inline const picture_t& getPicture() const { return mPicture; }
    inline const bool getVBlankNMISignal() const { return mVBlankNMI; }

private:
    u8 readByte(Memory& memory, u16 address);
    void writeByte(Memory& memory, u16 address, u8 value);

    void executeVisibleScanline(Memory& memory);
    void executePostRenderScanline(Memory& memory);
    void executeVBlankScanline(Memory& memory);
    void executePreRenderScanline(Memory& memory);

    void setPictureColor(u8 colorCode, u32 row, u32 col);

    // Rendering
    picture_t mPicture;
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
    bool mVBlankNMI;

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
