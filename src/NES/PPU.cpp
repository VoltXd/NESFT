#include "NES/PPU.hpp"

#include <random>

void PPU::reset()
{
    // Picture reset
    for (auto& line : mPicture)
        for (auto& pixel : line)
            for (u8& color : pixel)
                color = 0;

    // Registers reset
    mPpuCtrl    = 0b0000'0000;
    mPpuMask    = 0b0000'0000;
    mPpuStatus  = 0b1010'0000;
    mOamAddr    = 0b0000'0000;
    mPpuScrollX = 0b0000'0000; 
    mPpuScrollY = 0b0000'0000; 
    mPpuAddr    = 0b0000'0000'0000'0000;
    mPpuData    = 0b0000'0000;

    mW = 0;

    mIsOddFrame = false;

    mVBlankNMI = false;

    mCycleCount = 0;
    mScanlineCount = 0;

    mIsFirstPrerenderPassed = false;
}

void PPU::executeOneCycle(Memory &memory)
{
    // Execute one cycle
    if (mScanlineCount < 240)
        executeVisibleScanline(memory);

    else if (mScanlineCount == 240)
        executePostRenderScanline(memory);

    else if (mScanlineCount < 261)
        executeVBlankScanline(memory);

    else // if (mScanlineCount == 261)
        executePreRenderScanline(memory);

    // Incremement cycles & scanline count
    mCycleCount++;
    if (340 < mCycleCount)
    {
        mCycleCount = 0;

        mScanlineCount++;
        if (261 < mScanlineCount)
            mScanlineCount = 0;
    }

}

u8 PPU::readByte(Memory &memory, u16 address) 
{
    return memory.ppuRead(address);
}

void PPU::writeByte(Memory &memory, u16 address, u8 value)
{
    memory.ppuWrite(address, value);
}

void PPU::executeVisibleScanline(Memory &memory)
{
    if ((1 <= mCycleCount && mCycleCount <= 256) && (321 <= mCycleCount && mCycleCount <= 336))
    {
        // Get background pixels
        // (HW use two cycles but emulation will just fake it)
        if (((mCycleCount - 1) % 2) == 0)
            return;

        u16 address;
        if ((mCycleCount - 1) % 8 < 2)
        {
            // Get Nametable byte (aka tile index)
            address = 0x2000 | (mV & 0x0FFF);
            mBgData.nt = readByte(memory, address);
        }
        else if ((mCycleCount - 1) % 8 < 4)
        {
            // Get Attribute table byte (aka palettes indices)
            // reg.v is:      yyy'NNYY'YYYX'XXXX
            // AT address is: 010'NN11'11YY'YXXX (X & Y are MSb)
            address = 0x23C0 | (mV & 0x0C00) | ((mV >> 4) & 0x0038) | ((mV >> 2) & 0x0007);
            mBgData.at = readByte(memory, address);
        }
        else if ((mCycleCount - 1) % 8 < 6)
        {
            // Get tile LSB
            // Tile address is: H'NNNN'NNNN'0yyy
            address = ((u16)(mPpuCtrl & 0b0000'1000) << 9) |
                      ((u16)mBgData.nt << 4)               |
                      ((mV & 0x7000) >> 12)                &
                      0xFFF7; 
            mBgData.ptLsb = readByte(memory, address);
        }
        else
        {
            // Get tile MSB
            // Tile address is: H'NNNN'NNNN'1yyy
            address = ((u16)(mPpuCtrl & 0b0000'1000) << 9) |
                      ((u16)mBgData.nt << 4)               |
                      ((mV & 0x7000) >> 12)                |
                      0x0008; 
            mBgData.ptLsb = readByte(memory, address);

            // Update picture color ?
            // For each pixel:
            // Get color index (using pattern table bytes)
            // Get Palette (using AT byte & reg.v)
            // Get color code = palette[color_index]
            // Calculate real color
            // Magic (I hope...)
            for (int i = 0; i < 8; i++)
            {
                if (248 < mCycleCount && mCycleCount <= 256)
                    // Unused tile fetch
                    break;
                    
                u8 colorIdx;
                if (i == 0)
                    colorIdx = (mBgData.ptMsb & 1) << 1 | (mBgData.ptLsb & 1);
                else
                    colorIdx = (mBgData.ptMsb & (1 << i)) >> (i - 1) | (mBgData.ptLsb & (1 << i)) >> i;

                // Calculate Palette number
                bool isRightTile  = (mV & 0b0000'0000'0000'0010) != 0;
                bool isBottomTile = (mV & 0b0000'0000'0100'0000) != 0;

                u8 paletteNumber;
                if (!(isRightTile || isBottomTile))
                    paletteNumber = mBgData.at & 0b0000'0011;                

                else if (isRightTile && !isBottomTile)
                    paletteNumber = (mBgData.at & 0b0000'1100) >> 2;                
                    
                else if (!isRightTile && isBottomTile)
                    paletteNumber = (mBgData.at & 0b0011'0000) >> 4;                

                else
                    paletteNumber = (mBgData.at & 0b1100'0000) >> 6;                

                
                u16 colorAddress = 0x3F00 | (paletteNumber << 2) | colorIdx;
                u8 colorCode = readByte(memory, colorAddress);
                u32 row = mScanlineCount;
                u32 col = i + (mCycleCount > 256 ? mCycleCount - 321 : mCycleCount - 1 + 16) - 8;

                setPictureColor(colorCode, row, col);
            }

            // Increment reg.v
        }
    }
}

void PPU::executePostRenderScanline(Memory &memory)
{
    memory.ppuRead(0);
}

void PPU::executeVBlankScanline(Memory &memory)
{
    memory.ppuRead(0);
}

void PPU::executePreRenderScanline(Memory &memory)
{
    memory.ppuRead(0);
    mIsFirstPrerenderPassed = true;
}

void PPU::setPictureColor(u8 colorCode, u32 row, u32 col)
{
    const std::array<std::array<u8, 3>, 64> LUT = 
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

    mPicture[row][col][0] = LUT[colorCode][0];
    mPicture[row][col][1] = LUT[colorCode][1];
    mPicture[row][col][2] = LUT[colorCode][2];
}
