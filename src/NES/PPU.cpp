#include "NES/PPU.hpp"

#include <random>
#include <sstream>
#include <iomanip>
#include "NES/Toolbox.hpp"

void PPU::reset()
{
    // Picture reset
    for (auto& line : mPicture)
        for (auto& pixel : line)
            for (u8& color : pixel)
                color = 0;

    // Palette RAM reset
	for (u32 i = 0; i < PALETTE_RAM_SIZE; i++)
		mPaletteRam[i] = rand() % 0x40;
        
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
    mV = 0;

    mBgData = { 0 };

    mIsOddFrame = false;

    mOamSpriteIdx = 0;
    mOamByteIdx = 0;
    mIsStoringOamSprite = false;
    mIsNextLineSprite0InRenderBuffer = false;
    mIsSprite0InRenderBuffer = false;

    mVBlankNMISignal = false;
    mNMICanOccur = false;
    mIsImageReady = false;

    mCycleCount = 0;
    mScanlineCount = 0;

    mIsFirstPrerenderPassed = false;
}

void PPU::executeOneCycle(Memory &memory)
{
    // Log PPU internals
    logPpu();

    // Execute one cycle
    if (mScanlineCount < 240)
        executeVisibleScanline(memory);

    else if (mScanlineCount < 261)
        executeVBlankScanline();

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
            if (((mPpuMask & 0b0001'1000) != 0) && !(240 <= mScanlineCount && mScanlineCount < 261))
            {
                // Glitchy increment (high 6 bits only)
                mOamAddr += 4;
            }
            else
            {
                mOamData = value;
                mOam[mOamAddr] = mOamData;
                mOamAddr++;
            }
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

                // Dummy read to trigger MMC3 PPU A12 change
                readByte(memory, mV); 
            }
            mW = !mW;
            break;

        case PPUDATA_CPU_ADDR:
            writeByte(memory, mV, value);
            incrementOnPpudataEnding();
            readByte(memory, mV);
            break;

        // default:
        // {
        //     std::stringstream errorStream;
        //     errorStream << std::hex << "PPU register 0x" << address << " is not writeable.";
        //     testAndExitWithMessage(true, errorStream.str());
        // } break;
    }
}

u8 PPU::readRegister(Memory &memory, u16 address)
{
    u8 value = 0x00;
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
            if (mScanlineCount < 240 && 1 <= mCycleCount && mCycleCount <= 64)
                value = 0xFF;
            else
            {
                mOamData = mOam[mOamAddr];
                if ((mOamAddr % 4) == 2)
                    mOamData &= 0b1110'0011;
                value = mOamData;
            }
            break;
        
        case PPUDATA_CPU_ADDR:
            // PPUDATA READ BUFFER
            if ((mV & 0x3F00) == 0x3F00)
            {
                // Except for palette RAM
                value = readByte(memory, mV);
                mPpuData = value;
            }
            else
            {
                value = mPpuData;
                mPpuData = readByte(memory, mV);
            }
            incrementOnPpudataEnding();
            readByte(memory, mV);
            break;
        
        // default:
        // {
        //     std::stringstream errorStream;
        //     errorStream << std::hex << "PPU register 0x" << address << " is not readable.";
        //     testAndExitWithMessage(true, errorStream.str());
        // } break;
    }

	return value;
}

u8 PPU::readPaletteRam(u16 address)
{
    u16 paletteRamAddress = address;
    paletteRamAddress &= ((paletteRamAddress & 0x0003) == 0) ? 0x000F : 0x001F;
    u8 value = mPaletteRam[paletteRamAddress];
    return value;
}

void PPU::writePaletteRam(u16 address, u8 value)
{
    u16 paletteRamAddress = address;
    paletteRamAddress &= ((paletteRamAddress & 0x0003) == 0) ? 0x000F : 0x001F;
    mPaletteRam[paletteRamAddress] = value;
}

u8 PPU::readByte(Memory &memory, u16 address) 
{
    return memory.ppuRead(address, mCycleCount);
}

void PPU::writeByte(Memory &memory, u16 address, u8 value)
{
    memory.ppuWrite(address, value, mCycleCount);
}

void PPU::executeVisibleScanline(Memory &memory)
{
    if ((1 <= mCycleCount && mCycleCount <= 256) || (321 <= mCycleCount && mCycleCount <= 336))
    {
        // Get background pixels
        processPixelData(memory);
    
        // Increment reg.v: Y
        if (mCycleCount == 256)
            incrementY();
    }
    else if (257 <= mCycleCount && mCycleCount <= 320)
    {
        if (mCycleCount == 257)
        {
            // Load temp horizontal address
            loadX();
        }
        
        mOamAddr = 0;

        if (((mCycleCount % 2) == 0) && (((mCycleCount - 1) % 8) < 4))
        {
            u16 address = 0x2000 | (mV & 0x0FFF);
            mBgData.nt = readByte(memory, address);
        }
    }
    else if (mCycleCount > 336)
    {
        // Skip odd cycles
        if (mCycleCount % 2 != 0)
            return;

        // Dummy nametable fetches
        u16 address = 0x2000 | (mV & 0x0FFF);
        mBgData.nt = readByte(memory, address);
    }

    processSpriteEvaluation(memory);
}

void PPU::executeVBlankScanline()
{
    // Set VBlank flag & NMI
    if (mScanlineCount == 241 && mCycleCount == 1)
    {
        mNMICanOccur = true;
        mIsImageReady = true;
        mPpuStatus |= 0b1000'0000;
    }
}

void PPU::executePreRenderScanline(Memory &memory)
{
    memory; // Bypass "unused argument" warning
    
    mIsFirstPrerenderPassed = true;
    if (mCycleCount == 1)
    {
        mNMICanOccur = false;
        mPpuStatus &= ~0b1110'0000;
        mIsSprite0InRenderBuffer = false;
        mIsNextLineSprite0InRenderBuffer = false;
    }
    
    if (1 <= mCycleCount && mCycleCount <= 256)
    {
        // Dummy scanline fetches
        processPixelData(memory);

        // Increment reg.v: Y
        if (mCycleCount == 256)
            incrementY();
    }
    else if (mCycleCount == 257)
    {
        loadX();
    }
    else if (280 <= mCycleCount && mCycleCount <= 304)
    {
        loadY();
    }
    else if (321 <= mCycleCount && mCycleCount <= 336)
    {
        // Get background pixels
        processPixelData(memory);
    }
    else if (mCycleCount > 336)
    {
        // Skip odd cycles
        if (mCycleCount % 2 != 0)
            return;

        // Dummy nametable fetches
        u16 address = 0x2000 | (mV & 0x0FFF);
        mBgData.nt = readByte(memory, address);
    }

    if (mCycleCount == 339)
    {
        if (mIsOddFrame && ((mPpuMask & 0b0001'1000) != 0))
            mCycleCount++;

        mIsOddFrame = !mIsOddFrame;
    }

    if (257 <= mCycleCount && mCycleCount <= 320)
    {
        mOamAddr = 0;

        if (((mCycleCount % 2) == 0) && (((mCycleCount - 1) % 8) < 4))
        {
            u16 address = 0x2000 | (mV & 0x0FFF);
            mBgData.nt = readByte(memory, address);
        }
    }

    processSpriteEvaluation(memory);
}

void PPU::processPixelData(Memory& memory)
{
    // Fetch background data
    // (HW use two cycles but emulation will just fake it)
    if (((mCycleCount - 1) % 2) == 0)
        return;

    u16 address;
    u8 fetchCycle = (mCycleCount - 1) % 8;
    if (fetchCycle < 2)
    {
        // Get Nametable byte (aka tile index)
        address = 0x2000 | (mV & 0x0FFF);
        mBgData.nt = readByte(memory, address);
    }
    else if (fetchCycle < 4)
    {
        // Get Attribute table byte (aka palettes indices)
        // reg.v is:      yyy'NNYY'YYYX'XXXX
        // AT address is: 010'NN11'11YY'YXXX (X & Y are MSbits)
        address = 0x23C0 | (mV & 0x0C00) | ((mV >> 4) & 0x0038) | ((mV >> 2) & 0x0007);
        mBgData.at = readByte(memory, address);
    }
    else if (fetchCycle < 6)
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

        // Update picture color (background & sprites)
        // For each pixel:
        // Get color index (using pattern table bytes)
        // Get Palette (using AT byte & reg.v)
        // Get color code = palette[color_index]
        // Calculate real color
        // Magic (I hope...)
        for (u8 i = 0; i < 8; i++)
        {
            if (248 < mCycleCount && mCycleCount <= 256)
                // Unused tile fetch
                break;

            if (mScanlineCount == 239 && mCycleCount > 256)
                // Unused tile fetch
                break;
            
            u16 row = 0;
            if (mScanlineCount != 261)
                row = mCycleCount > 256 ? mScanlineCount + 1 : mScanlineCount;
            u16 col = i + (mCycleCount > 256 ? mCycleCount - 321 - 7 : mCycleCount + 8) - mX;

            bool hasSpritePriority = false;
            u16 bgColorAddress = getColorAddressFromBGData(i, col);
            u16 spriteColorAddress = getColorAddressFromSecOam((u8)col, (u8)row, hasSpritePriority);

            // Sprite 0 hit
            if (!(((mPpuMask & 0b0001'1000) != 0b0001'1000) ||
                  (0 <= col && col < 8 && ((mPpuMask & 0b0000'0110) != 0b0000'0110)) ||
                  (col == 255)))
            {
                // Sprite 0 hit is possible
                u16 sprite0ColorAddress;
                bool isSprite0Detected = isSprite0OnPixel((u8)col, (u8)row, sprite0ColorAddress);
                if (isSprite0Detected)
                {
                    // Check BG presence
                    if ((bgColorAddress & 0x0003) != 0)
                    {
                        // Sprite 0 hit detected
                        mPpuStatus |= 0b0100'0000;
                    }
                }
            }

            u16 colorAddress = 0x3F00;
            if (((bgColorAddress | spriteColorAddress) & 0x0003) == 0)
            {
                // No BG or sprite color -> Backdrop color
                colorAddress = 0x3F00;
            }
            else if ((bgColorAddress & 0x0003) == 0)
            {
                // No BG color -> Sprite color
                colorAddress = spriteColorAddress;
            }
            else if ((spriteColorAddress & 0x0003) == 0)
            {
                // No sprite color -> BG color
                colorAddress = bgColorAddress;
            }
            else 
            {
                // BG & sprite have a color
                if (hasSpritePriority)
                {
                    // Sprite has priority over BG -> Sprite color
                    colorAddress = spriteColorAddress;
                }
                else
                {
                    // BG has priority over sprite -> BG color
                    colorAddress = bgColorAddress;
                }
            }

            u8 colorCode = readPaletteRam(colorAddress);
            setPictureColor(colorCode, row, col);
        }

        // Increment reg.v: Coarse X 
        incrementCoarseX();
    }
}

void PPU::processSpriteEvaluation(Memory &memory)
{
    memory; // Bypass "argument unused" warning

    // No sprite evaluation in pre-render scanline
    if ((mCycleCount < 257) && (mScanlineCount == 261))
        return;
     
    bool isOddCycle = (mCycleCount % 2) == 1;
    if (mCycleCount == 0)
    {
        // Reset counters
        mSecOamIdx = 0;
        mOamByteIdx = 0;
        mOamSpriteIdx = 0;
        mIsStoringOamSprite = false;
        mIsNextLineSprite0InRenderBuffer = false;
    }
    else if (1 <= mCycleCount && mCycleCount <= 64)
    {
        // Clear secondary OAM
        if (!isOddCycle)
            return;
        
        u8 soamIdx = (u8)mCycleCount / 2;
        mOamSecondary[soamIdx] = 0xFF;
    }
    else if (65 <= mCycleCount && mCycleCount <= 256)
    {
        if (mOamSpriteIdx == 64)
            // No reading or writing if all sprites are read
            return;

        if (isOddCycle)
        {
            // Read from primary OAM
            u8 oamIdx = 4 * mOamSpriteIdx + mOamByteIdx;
            mOamTransfertBuffer = mOam[oamIdx];
        }
        else
        {
            // Write into secondary OAM
            u8 spriteHeight = ((mPpuCtrl & 0b0010'0000) == 0) ? 8 : 16;
            u16 pixelY = mScanlineCount;
            if (mSecOamIdx == 8 * 4)
            {
                // Check for Sprite overflow (+ HW bug)
                if (!mIsStoringOamSprite)
                {
                    if (mOamTransfertBuffer <= pixelY && pixelY < mOamTransfertBuffer + spriteHeight)
                    {
                        // Set sprite overflow flag
                        if ((mPpuMask & 0b0001'1000) != 0)
                            mPpuStatus |= 0b0010'0000;
                        mIsStoringOamSprite = true;
                        mOamByteIdx++;
                    }
                    else
                    {
                        // Hardware bug
                        mOamSpriteIdx++;
                        mOamByteIdx++;
                        if (mOamByteIdx == 4)
                            mOamByteIdx = 0;
                    }
                }
                else
                {
                    mOamByteIdx++;
                    if (mOamByteIdx == 4)
                    {
                        // Finished fetching sprite
                        mOamByteIdx = 0;
                        mOamSpriteIdx++;
                        mIsStoringOamSprite = false;
                    }
                }

                // No writing if secondary OAM is full (8 sprites)
                return;
            }
            
            mOamSecondary[mSecOamIdx] = mOamTransfertBuffer;

            if (!mIsStoringOamSprite)
            {
                // Check that Y position fits to next scanline
                if (mOamTransfertBuffer <= pixelY && pixelY < mOamTransfertBuffer + spriteHeight)
                {
                    // It fits -> get sprites 4 bytes
                    if (mOamSpriteIdx == 0)
                        mIsNextLineSprite0InRenderBuffer = true;
                    
                    mIsStoringOamSprite = true;
                    mSecOamIdx++;
                    mOamByteIdx++;
                }
                else
                {
                    // It doesn't fit -> keep fetching Y pos bytes
                    mOamSpriteIdx++;
                }
            }
            else
            {
                // Increment current sprite byte counter & sec OAM counter
                mSecOamIdx++;
                mOamByteIdx++;
                if (mOamByteIdx == 4)
                {
                    // Finished fetching sprite
                    mOamByteIdx = 0;
                    mOamSpriteIdx++;
                    mIsStoringOamSprite = false;
                }
            }
        }
    }
    else if (257 <= mCycleCount && mCycleCount <= 320)
    {
        if (mCycleCount == 257)
        {
            mIsSprite0InRenderBuffer = mIsNextLineSprite0InRenderBuffer;
            mSpriteRenderBuffer = mOamSecondary;
        }

        mOamAddr = 0;

        // Fetch Sprite patterns
        // Skip odd cycles
        if (isOddCycle)
            return;
        
        u16 spriteCycleCount = mCycleCount - 257;

        // Skip dummy fetches
        if ((spriteCycleCount % 8) < 4)
            return;
        
        // Calculate pattern buffer index
        u16 patternBufferIdx = 2 * (spriteCycleCount / 8) + ((spriteCycleCount % 4) / 2);

        // Get sprite information
        u8 spriteHeight = ((mPpuCtrl & 0b0010'0000) == 0) ? 8 : 16;
        u8 yPos      = mSpriteRenderBuffer[(patternBufferIdx / 2) * 4 + 0];
        u8 tileIdx   = mSpriteRenderBuffer[(patternBufferIdx / 2) * 4 + 1];
        u8 attribute = mSpriteRenderBuffer[(patternBufferIdx / 2) * 4 + 2];
        u8 pixelYPos = (u8)mScanlineCount;
        u16 address;
    
        if ((mPpuCtrl & 0b0010'0000) == 0)
        {
            // Pattern table (8x8)
            address = ((u16)mPpuCtrl & 0b0000'1000) << 9 |
                      (u16)tileIdx << 4;
        }
        else
        {
            // Pattern table (16x8)
            address = ((u16)tileIdx & 0b0000'0001) << 12 | 
                      ((u16)tileIdx & 0b1111'1110) << 4;
        }

        // Vertical flip
        u8 spriteYPos = (u8)(((attribute & 0b1000'0000) == 0) ? 
                        (pixelYPos - yPos) : 
                        (spriteHeight - 1 - (pixelYPos - yPos)));
        if (spriteHeight == 8)
            address |= spriteYPos;
        else
            address |= ((spriteYPos & 0b0000'1000) << 1) | (spriteYPos & 0b0000'0111); 
        
        // Get pattern table byte
        u8 spritePattern = ((patternBufferIdx % 2) == 0) ?
                           readByte(memory, address) :
                           readByte(memory, address | 0b0000'0000'0000'1000);
        mSpritePatternBuffer[patternBufferIdx] = spritePattern;
    }
}

void PPU::setPictureColor(u8 colorCode, u16 row, u16 col) 
{
    if (col >= PPU_OUTPUT_WIDTH)
        return;

    mPicture[row][col][0] = LUT[colorCode][0];
    mPicture[row][col][1] = LUT[colorCode][1];
    mPicture[row][col][2] = LUT[colorCode][2];
}

u16 PPU::getColorAddressFromBGData(u8 xIdx, u16 pixelX)
{
    // Return EXT input if leftmost 8 pixels and PPUMASK.1 == 0
    if ((pixelX < 8) && ((mPpuMask & 0b0000'0010) == 0))
        return 0x3F00;

    // Get 2 bits color index using pattern table byte
    u8 colorIdx = getColorIndexFromPattern(mBgData.ptLsb, mBgData.ptMsb, xIdx);

    // Calculate Palette number (which palette is used)
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

    // Get color (in palette) address -> Get color code -> Set pixel color
    u16 colorAddress = 0x3F00; 
    if ((mPpuMask & 0b0000'1000) != 0)
        // BG Rendering Enable
        colorAddress |= (paletteNumber << 2) | colorIdx;

    return colorAddress;
}

u16 PPU::getColorAddressFromSecOam(u16 pixelXPos, u16 pixelYPos, bool& hasSpritePriority)
{
    // Return EXT input when no rendering
    if ((mPpuMask & 0b0001'0000) == 0)
        return 0x3F00;
        
    oamData sprite;
    u8 ptLsb;
    u8 ptMsb;
    for (u8 spriteIdx = 0; spriteIdx < 8; spriteIdx++)
    {
        sprite.yPos      = mSpriteRenderBuffer[spriteIdx * 4 + 0];
        sprite.tileIdx   = mSpriteRenderBuffer[spriteIdx * 4 + 1];
        sprite.attribute = mSpriteRenderBuffer[spriteIdx * 4 + 2];
        sprite.xPos      = mSpriteRenderBuffer[spriteIdx * 4 + 3];
        ptLsb = mSpritePatternBuffer[spriteIdx * 2];
        ptMsb = mSpritePatternBuffer[spriteIdx * 2 + 1];

        u16 colorAddress = getColorAddressFromSprite(sprite, ptLsb, ptMsb, pixelXPos, pixelYPos, hasSpritePriority);
        if (colorAddress == SPRITE_NOT_IN_RANGE)
            continue;

        if ((colorAddress & 0x0003) == 0)
            // Sprite pixel is transparent
            continue;
        
        return colorAddress;
    }

    return 0x3F00;
}

u16 PPU::getColorAddressFromSprite(oamData sprite, u8 ptLsb, u8 ptMsb, u16 pixelXPos, u16 pixelYPos, bool &hasSpritePriority)
{
    // Return EXT input if leftmost 8 pixels and PPUMASK.2 == 0
    if ((pixelXPos < 8) && ((mPpuMask & 0b0000'0100) == 0))
        return 0x3F00;

    u8 spriteHeight = ((mPpuCtrl & 0b0010'0000) == 0) ? 8 : 16;
    u16 yPos = sprite.yPos + 1;
    if (!(yPos <= pixelYPos && pixelYPos < yPos + spriteHeight))
        return SPRITE_NOT_IN_RANGE;

    // Sprite is rendered on this scanline
    u8 spriteXPos = sprite.xPos;

    if (!(spriteXPos <= pixelXPos && pixelXPos < spriteXPos + 8))
        return SPRITE_NOT_IN_RANGE;
    
    // Sprite is render on this pixel
    u8 attribute  = sprite.attribute;

    // Get sprite priority
    hasSpritePriority = (attribute & 0b0010'0000) == 0;

    u8 xIdx = (u8)(pixelXPos - spriteXPos);

    // Horizontal flip
    if ((attribute & 0b0100'0000) != 0)
        xIdx = 7 - xIdx;

    u8 colorIdx = getColorIndexFromPattern(ptLsb, ptMsb, xIdx);

    // Palette 
    u8 paletteNumber = attribute & 0b0000'0011;

    return 0x3F10 | (paletteNumber << 2) | colorIdx; 
}

u8 PPU::getColorIndexFromPattern(u8 ptLsb, u8 ptMsb, u8 xIdx)
{
    u8 colorIdx;
    if (xIdx != 7)
        colorIdx = (ptMsb & (1 << (7 - xIdx))) >> (7 - xIdx - 1) | (ptLsb & (1 << (7 - xIdx))) >> (7 - xIdx);
    else
        colorIdx = (ptMsb & 1) << 1 | (ptLsb & 1);

    return colorIdx;
}

bool PPU::isSprite0OnPixel(u8 pixelXPos, u8 pixelYPos, u16 &colorAddress)
{
    // Sprite 0 is not in sprite rendering buffer
    if (!mIsSprite0InRenderBuffer)
        return false;

    bool dummyFlag;
    oamData sprite;
    u8 ptLsb = mSpritePatternBuffer[0];
    u8 ptMsb = mSpritePatternBuffer[1];
    sprite.yPos      = mSpriteRenderBuffer[0];
    sprite.tileIdx   = mSpriteRenderBuffer[1];
    sprite.attribute = mSpriteRenderBuffer[2];
    sprite.xPos      = mSpriteRenderBuffer[3];
    colorAddress = getColorAddressFromSprite(sprite, ptLsb, ptMsb, pixelXPos, pixelYPos, dummyFlag);
    if (colorAddress == SPRITE_NOT_IN_RANGE)
        return false;
    
    if ((colorAddress & 0x0003) == 0)
        // Sprite pixel is transparent
        return false;
    
    return true;
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

void PPU::incrementOnPpudataEnding()
{
    if ((mScanlineCount < 240 || mScanlineCount == 261) && ((mPpuMask & 0b0001'1000) != 0))
    {
        // While rendering, increment with hardware bug
        incrementCoarseX();
        incrementY();
    }
    else
    {
        // Normal increment
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
}

void PPU::logPpu()
{
    if (!gIsTraceLogPpuEnabled)
		return;

	std::stringstream ppuTraceStream;
	ppuTraceStream << "Scanline:" << mScanlineCount 
                   << ", Cycle:" << mCycleCount 
	               << ", (V, T, W):($" << std::uppercase << std::hex 
                   << std::setw(4) << mV << ", $" 
                   << std::setw(4) << +mT << ", $" 
                   << std::setw(1) << +mW << ")\n";
	traceLog(ppuTraceStream.str());
}
