#include "NES/PPU.hpp"

#include <random>
#include <sstream>
#include "NES/Toolbox.hpp"

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

    mVBlankNMISignal = false;
    mNMICanOccur = false;
    mIsImageReady = false;

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

    if (((mPpuCtrl & 0x80) != 0) && mNMICanOccur)
        mVBlankNMISignal = true;

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

void PPU::writeRegister(Memory& memory, u16 address, u8 value)
{
    switch (address)
    {
        case PPUCTRL_CPU_ADDR:
            // Ignore writes until first pre-render scanline
            if (!mIsFirstPrerenderPassed)
                break;
            mPpuCtrl = value;
            mT = (mT & 0b111'0011'1111'1111) | ((u16)(mPpuCtrl & 0b0000'0011) << 10);
            break;

        case PPUMASK_CPU_ADDR:
            // Ignore writes until first pre-render scanline
            if (!mIsFirstPrerenderPassed)
                break;
            mPpuMask = value;
            break;

        case OAMADDR_CPU_ADDR:
            mOamAddr = value;
            break;

        case OAMDATA_CPU_ADDR:
            // TODO: store in OAM
            mOamData = value;
            mOam[mOamAddr] = mOamData;
            mOamAddr++;
            break;

        case PPUSCROLL_CPU_ADDR:
            if (mW == 0)
            {
                mPpuScrollX = value;
                mT = (mT & 0b111'1111'1110'0000) | ((u16)(mPpuScrollX & 0b1111'1000) >> 3);
                mX = mPpuScrollX & 0b0000'0111;
            }
            else
            {
                mPpuScrollY = value;
                mT = (mT & 0b000'1100'0001'1111) | ((u16)(mPpuScrollY & 0b1111'1000) << 2) | ((u16)(mPpuScrollY & 0b0000'0111) << 12);
            }
            mW = !mW;
            break;
            
        case PPUADDR_CPU_ADDR:
            if (mW == 0)
            {
                mPpuAddr = (u16)(value & 0b0011'1111) << 8;
                mT = (mT & 0b000'0000'1111'1111) | ((u16)(value & 0b0011'1111) << 8);
            }
            else
            {
                mPpuAddr |= value;
                mT = (mT & 0b011'1111'0000'0000) | value;
                mV = mT;
            }
            mW = !mW;
            break;

        case PPUDATA_CPU_ADDR:
            writeByte(memory, mV, value);

            // TODO: weird behaviour while rendering
            if ((mScanlineCount < 240 || mScanlineCount == 261) && ((mPpuMask & 0b0001'1000) != 0))
            {
                incrementCoarseX();
                incrementY();
            }
            else
            {
                if ((mPpuCtrl & 0x04) == 0)
                {
                    mPpuAddr++;
                    mV++;
                }
                else
                {
                    mPpuAddr += 0b10'0000;
                    mV += 0b10'0000;
                }
            }
            break;

        default:
        {
            std::stringstream errorStream;
            errorStream << std::hex << "Register " + std::to_string(address) + " is not writeable.";
            testAndExitWithMessage(true, errorStream.str());
        } break;
    }
}

u8 PPU::readRegister(Memory &memory, u16 address)
{
    u8 value = 0xFF;
    switch (address)
    {
        case PPUSTATUS_CPU_ADDR:
            // TODO: Resets (see NESwiki)
            value = (mPpuStatus & 0b1110'0000) | (mPpuData & 0b0001'1111);
            mPpuStatus &= ~0b1000'0000;
            mNMICanOccur = false;
            mVBlankNMISignal = false;
            mW = 0;
            break;
        
        case OAMDATA_CPU_ADDR:
            mOamData = mOam[mOamAddr];
            value = mOamData;
            break;
        
        case PPUDATA_CPU_ADDR:
            // TODO: see NESwiki if more
            // PPUDATA BUFFER
            if ((mV & 0x3F00) == 0x3F00)
            {
                value = readByte(memory, mV);
                mPpuData = value;
            }
            else
            {
                value = mPpuData;
                mPpuData = readByte(memory, mV);
            }

            if ((mScanlineCount < 240 || mScanlineCount == 261) && ((mPpuMask & 0b0001'1000) != 0))
            {
                incrementCoarseX();
                incrementY();
            }
            else
            {
                if ((mPpuCtrl & 0x04) == 0)
                {
                    mPpuAddr++;
                    mV++;
                }
                else
                {
                    mPpuAddr += 0b10'0000;
                    mV += 0b10'0000;
                }
            }
            break;
        
        default:
        {
            std::stringstream errorStream;
            errorStream << std::hex << "Register " + std::to_string(address) + " is not readable.";
            testAndExitWithMessage(true, errorStream.str());
        } break;
    }

	return value;
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
    if ((1 <= mCycleCount && mCycleCount <= 256) || (321 <= mCycleCount && mCycleCount <= 336))
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
            address = (((u16)(mPpuCtrl & 0b0001'0000) << 8) |
                      ((u16)mBgData.nt << 4)                |
                      ((mV & 0x7000) >> 12))                &
                      0xFFF7; 
            mBgData.ptLsb = readByte(memory, address);
        }
        else
        {
            // Get tile MSB
            // Tile address is: H'NNNN'NNNN'1yyy
            address = ((u16)(mPpuCtrl & 0b0001'0000) << 8) |
                      ((u16)mBgData.nt << 4)               |
                      ((mV & 0x7000) >> 12)                |
                      0x0008; 
            mBgData.ptMsb = readByte(memory, address);

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

                if (mScanlineCount == 239 && mCycleCount > 256)
                    // Unused tile fetch
                    break;
                    
                if ((mPpuMask & 0b0000'1000) == 0)
                    // Only when rendering
                    break;
                    
                u8 colorIdx;
                if (i != 7)
                    colorIdx = (mBgData.ptMsb & (1 << (7 - i))) >> (7 - i - 1) | (mBgData.ptLsb & (1 << (7 - i))) >> (7 - i);
                else
                    colorIdx = (mBgData.ptMsb & 1) << 1 | (mBgData.ptLsb & 1);

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
                u32 row = mCycleCount > 256 ? mScanlineCount + 1 : mScanlineCount;
                u32 col = i + (mCycleCount > 256 ? mCycleCount - 321 - 7 : mCycleCount + 8) - mX;

                setPictureColor(colorCode, row, col);
            }

            // Increment reg.v: Coarse X 
            incrementCoarseX();
        }
        
        // Increment reg.v: Y
        if (mCycleCount == 256)
            incrementY();
    }
    else if (mCycleCount == 257)
    {
        // Load temp horizontal address
        loadX();
    }
}

void PPU::executePostRenderScanline(Memory &memory)
{
    // Idle scan line
    memory.ppuRead(0);
}

void PPU::executeVBlankScanline(Memory &memory)
{
    // Set VBlank flag & NMI
    memory.ppuRead(0);
    if (mScanlineCount == 241 && mCycleCount == 1)
    {
        mNMICanOccur = true;
        mIsImageReady = true;
        mPpuStatus |= 0b1000'0000;
    }
}

void PPU::executePreRenderScanline(Memory &memory)
{
    memory.ppuRead(0);
    mIsFirstPrerenderPassed = true;
    if (mCycleCount == 1)
    {
        mNMICanOccur = false;
        mPpuStatus &= ~0b1000'0000;
    }
    
    if (1 <= mCycleCount && mCycleCount <= 256)
    {
        if ((mCycleCount - 1) % 8 == 7)
            // Increment reg.v: Coarse X (the same increments as visible scanlines
            //                            are made)
            incrementCoarseX();
        
        // Increment reg.v: Y
        if (mCycleCount == 256)
            incrementY();
    }
    else if (mCycleCount == 257)
        loadX();
    else if (280 <= mCycleCount && mCycleCount <= 304)
        loadY();
    else if (321 <= mCycleCount && mCycleCount <= 336)
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
            address = ((u16)(mPpuCtrl & 0b0001'0000) << 8) |
                      ((u16)mBgData.nt << 4)               |
                      ((mV & 0x7000) >> 12)                &
                      0xFFF7; 
            mBgData.ptLsb = readByte(memory, address);
        }
        else
        {
            // Get tile MSB
            // Tile address is: H'NNNN'NNNN'1yyy
            address = ((u16)(mPpuCtrl & 0b0001'0000) << 8) |
                      ((u16)mBgData.nt << 4)               |
                      ((mV & 0x7000) >> 12)                |
                      0x0008; 
            mBgData.ptMsb = readByte(memory, address);

            // Update picture color ?
            // For each pixel:
            // Get color index (using pattern table bytes)
            // Get Palette (using AT byte & reg.v)
            // Get color code = palette[color_index]
            // Calculate real color
            // Magic (I hope...)
            for (int i = 0; i < 8; i++)
            {
                if ((mPpuMask & 0b0000'1000) == 0)
                    // Only when rendering
                    break;

                u8 colorIdx;
                if (i != 7)
                    colorIdx = (mBgData.ptMsb & (1 << (7 - i))) >> (7 - i - 1) | (mBgData.ptLsb & (1 << (7 - i))) >> (7 - i);
                else
                    colorIdx = (mBgData.ptMsb & 1) << 1 | (mBgData.ptLsb & 1);

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
                u32 row = 0;
                u32 col = i + mCycleCount - 321 - 7 - mX;

                setPictureColor(colorCode, row, col);
            }

            // Increment reg.v: Coarse X 
            incrementCoarseX();
        }
    }
    else if (mCycleCount == 339)
    {
        if (mIsOddFrame)
            mCycleCount++;

        mIsOddFrame = !mIsOddFrame;
    }
}

void PPU::setPictureColor(u8 colorCode, u32 row, u32 col)
{
    if (col >= PPU_OUTPUT_WIDTH)
        return;

    mPicture[row][col][0] = LUT[colorCode][0];
    mPicture[row][col][1] = LUT[colorCode][1];
    mPicture[row][col][2] = LUT[colorCode][2];
}

void PPU::incrementCoarseX()
{
    if ((mPpuMask & 0b0001'1000) == 0)
        // Only when rendering
        return;

    if ((mV & 0x001F) == 31)
    {
        // Coarse X overflow: set to 0 + switch horizontal Nametable
        mV &= ~0x001F;
        mV ^=  0x0400;
    }
    else
        mV++;
}

void PPU::incrementY()
{
    if ((mPpuMask & 0b0001'1000) == 0)
        // Only when rendering
        return;
        
    if ((mV & 0x7000) != 0x7000)
        // Increment fine Y
        mV += 0x1000;
    else
    {
        // Fine Y overflow: set to 0 + increment coarse Y
        mV &= ~0x7000;
        u8 coarseY = (u8)((mV & 0x03E0) >> 5);
        if (coarseY == 29)
        {
            // Coarse Y overvlow: set to 0 + switch nametable
            coarseY = 0;
            mV ^= 0x0800;
        }
        else if (coarseY == 31)
            // Coarse Y forbidden value: set to 0
            coarseY = 0;
        else
            coarseY++;

        mV = (mV & ~0x03E0) | (coarseY << 5);
    }
}

void PPU::loadX()
{
    if ((mPpuMask & 0b0001'1000) == 0)
        // Only when rendering
        return;
    
    mV = (mV & 0b111'1011'1110'0000) | (mT & ~0b111'1011'1110'0000);
}

void PPU::loadY()
{
    if ((mPpuMask & 0b0001'1000) == 0)
        // Only when rendering
        return;

    mV = (mV & ~0b111'1011'1110'0000) | (mT & 0b111'1011'1110'0000);
}
