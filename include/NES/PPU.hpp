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

    u8 readPaletteRam(u16 address);
    void writePaletteRam(u16 address, u8 value);

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
    u16 getColorAddressFromSecOam(Memory& memory, u8 pixelXPos, u8 pixelYPos, bool& hasSpritePriority);
    u16 getColorAddressFromSprite(Memory& memory, oamData sprite, u8 pixelXPos, u8 pixelYPos, bool& hasSpritePriority);
    u8 getColorIndexFromPattern(u8 ptLsb, u8 ptMsb, u8 xIdx);

    bool isSprite0OnPixel(Memory& memory, u8 pixelXPos, u8 pixelYPos, u16& colorAddress);

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
	static constexpr u32 PALETTE_RAM_SIZE = 0x0020; // 32 B
    std::array<u8, PALETTE_RAM_SIZE> mPaletteRam;

    // NMI
    bool mVBlankNMISignal;
    bool mNMICanOccur;

    // Color Lookup table
    static constexpr std::array<std::array<u8, 3>, 64> LUT = 
    {{
        { 0x66, 0x66, 0x66 }, 
        { 0x00, 0x2a, 0x88 }, 
        { 0x14, 0x12, 0xa7 }, 
        { 0x3b, 0x00, 0xa4 }, 
        { 0x5c, 0x00, 0x7e }, 
        { 0x6e, 0x00, 0x40 }, 
        { 0x6c, 0x06, 0x00 }, 
        { 0x56, 0x1d, 0x00 }, 
        { 0x33, 0x35, 0x00 }, 
        { 0x0b, 0x48, 0x00 }, 
        { 0x00, 0x52, 0x00 }, 
        { 0x00, 0x4f, 0x08 }, 
        { 0x00, 0x40, 0x4d }, 
        { 0x00, 0x00, 0x00 }, 
        { 0x00, 0x00, 0x00 },
        { 0x00, 0x00, 0x00 }, 
        { 0xad, 0xad, 0xad }, 
        { 0x15, 0x5f, 0xd9 }, 
        { 0x42, 0x40, 0xff }, 
        { 0x75, 0x27, 0xfe }, 
        { 0xa0, 0x1a, 0xcc }, 
        { 0xb7, 0x1e, 0x7b }, 
        { 0xb5, 0x31, 0x20 }, 
        { 0x99, 0x4e, 0x00 }, 
        { 0x6b, 0x6d, 0x00 }, 
        { 0x38, 0x87, 0x00 }, 
        { 0x0c, 0x93, 0x00 }, 
        { 0x00, 0x8f, 0x32 }, 
        { 0x00, 0x7c, 0x8d }, 
        { 0x00, 0x00, 0x00 }, 
        { 0x00, 0x00, 0x00 }, 
        { 0x00, 0x00, 0x00 }, 
        { 0xff, 0xfe, 0xff }, 
        { 0x64, 0xb0, 0xff }, 
        { 0x92, 0x90, 0xff }, 
        { 0xc6, 0x76, 0xff }, 
        { 0xf3, 0x6a, 0xff }, 
        { 0xfe, 0x6e, 0xcc }, 
        { 0xfe, 0x81, 0x70 }, 
        { 0xea, 0x9e, 0x22 }, 
        { 0xbc, 0xbe, 0x00 }, 
        { 0x88, 0xd8, 0x00 }, 
        { 0x5c, 0xe4, 0x30 }, 
        { 0x45, 0xe0, 0x82 }, 
        { 0x48, 0xcd, 0xde }, 
        { 0x4f, 0x4f, 0x4f }, 
        { 0x00, 0x00, 0x00 }, 
        { 0x00, 0x00, 0x00 }, 
        { 0xff, 0xfe, 0xff }, 
        { 0xc0, 0xdf, 0xff }, 
        { 0xd3, 0xd2, 0xff }, 
        { 0xe8, 0xc8, 0xff }, 
        { 0xfb, 0xc2, 0xff }, 
        { 0xfe, 0xc4, 0xea }, 
        { 0xfe, 0xcc, 0xc5 }, 
        { 0xf7, 0xd8, 0xa5 }, 
        { 0xe4, 0xe5, 0x94 }, 
        { 0xcf, 0xef, 0x96 }, 
        { 0xbd, 0xf4, 0xab }, 
        { 0xb3, 0xf3, 0xcc }, 
        { 0xb5, 0xeb, 0xf2 }, 
        { 0xb8, 0xb8, 0xb8 }, 
        { 0x00, 0x00, 0x00 }, 
        { 0x00, 0x00, 0x00 }
    }};
};
