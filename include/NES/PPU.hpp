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
    struct backgroundData
    {
        u8 nt;
        u8 at;
        u8 ptLsb;
        u8 ptMsb;
    };

    struct oamData
    {
        u8 yPos;
        u8 tileIdx;
        u8 attribute;
        u8 xPos;
    };

    u8 readByte(Memory& memory, u16 address);
    void writeByte(Memory& memory, u16 address, u8 value);

    void executeVisibleScanline(Memory& memory);
    void executeVBlankScanline();
    void executePreRenderScanline(Memory& memory);

    void processPixelData(Memory& memory);
    void processSpriteEvaluation(Memory& memory);
    void setPictureColor(u8 colorCode, u32 row, u32 col);

    u16 getColorAddressFromBGData(u8 xIdx);
    u16 getColorAddressFromSecOam(Memory& memory, u8 pixelXPos, bool& hasSpritePriority);
    u16 getColorAddressFromSprite(Memory& memory, oamData sprite, u8 pixelXPos, bool& hasSpritePriority);
    u8 getColorIndexFromPattern(u8 ptLsb, u8 ptMsb, u8 xIdx);

    bool isSprite0OnPixel(Memory& memory, u8 pixelXPos, u16& colorAddress);

    void incrementCoarseX();
    void incrementY();

    void loadX();
    void loadY();

    void incrementOnPpudataEnding();

    // Rendering
    picture_t mPicture;
    bool mIsImageReady;
    u16 mScanlineCount;
    u16 mCycleCount;

    backgroundData mBgData;

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
    static constexpr u16 SPRITE_NOT_IN_RANGE = 0xFFFF;
    std::array<u8, 256> mOam;
    std::array<u8, 32> mOamSecondary = {{ 0xFF }};
    std::array<u8, 32> mSpriteRenderBuffer = {{ 0xFF }};
    u8 mOamTransfertBuffer;
    u8 mOamSpriteIdx;
    u8 mOamByteIdx;
    u8 mSecOamIdx;
    bool mIsStoringOamSprite;

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
