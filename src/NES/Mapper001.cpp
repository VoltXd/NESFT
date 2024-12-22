#include "NES/Mapper001.hpp"

Mapper001::Mapper001(u8 prgNumBanks, u8 chrNumBanks)
	: Mapper(prgNumBanks, chrNumBanks, NametableArrangement::VERT)
{
	mIsChrRamSelected = (chrNumBanks == 0);
}

void Mapper001::reset()
{
	resetShiftRegister();

	// Set PRG-ROM mode to 3
	mPrgBankMode = 3;
}

void Mapper001::resetShiftRegister()
{
	// Reset the shift register
	mCpuShiftRegister = 0;
	mCpuShiftCount = 0;
}

bool Mapper001::mapCpuWrite(u16 address, u32 &mappedAddress, u8 value)
{
	// Is the cartridge targeted ?
	if (address < 0x6000)
		return false;
	
	// Is the PRG-RAM targeted ?
	else if (address < 0x8000)
	{
		// TODO: map PRG-RAM
		mappedAddress = address & 0x1FFF;
		return true;
	}

	// The mapper registers are targeted
	u8 bitValue = value & 0x01;
	bool isResetTriggered = (value & 0x80) != 0;

	// Is reset triggered ?
	if (isResetTriggered)
	{
		reset();
		return false;
	}
	
	// No reset -> fill the shift register
	mCpuShiftRegister |= bitValue << mCpuShiftCount;
	mCpuShiftCount++;

	// Process shift register content when full
	if (mCpuShiftCount == SHIFT_REGISTER_SIZE)
		processShiftRegister(address);
		
	return false;
}

bool Mapper001::mapCpuRead(u16 address, u32 &mappedAddress)
{
	// Is the cartridge targeted ?
	if (address < 0x6000)
		return false;

	// Is PRG-RAM targeted ? 
	else if (address < 0x8000)
	{
		// PRG-RAM
		mappedAddress = address & 0x1FFF;
		mIsPrgRamRead = true;
		return true;
	}

	// PRG-ROM is targeted
	switch (mPrgBankMode)
	{
		case 0:
		case 1:
			mappedAddress = ((mPrgBankIdx & 0b1110) << 14) | (0x7FFF & address);
			break;
		
		case 2:
			if (address < 0xC000)
			{
				// Fixed to first bank
				mappedAddress = 0x3FFF & address;
			}
			else
			{
				// PRG bank
				mappedAddress = ((mPrgBankIdx & 0b1111) << 14) | (0x3FFF & address);
			}
			break;

		case 3:
			if (address < 0xC000)
			{
				// PRG bank
				mappedAddress = ((mPrgBankIdx & 0b1111) << 14) | (0x3FFF & address);
			}
			else
			{
				// Fixed to last bank
				const u32 LAST_BANK_OFFSET = ((mPrgNumBanks - 1) << 14);
				mappedAddress = LAST_BANK_OFFSET | (0x3FFF & address);
			}
			break;
	}

	mIsPrgRamRead = false;
	return true;
}

bool Mapper001::mapPpuWrite(u16 address, u32 &mappedAddress)
{
	return mapPpuAddress(address, mappedAddress);
}

bool Mapper001::mapPpuRead(u16 address, u32 &mappedAddress)
{
	return mapPpuAddress(address, mappedAddress);
}

void Mapper001::processShiftRegister(u16 address)
{
	constexpr u16 ADDRESS_MASK    = 0x6000;
	constexpr u16 CONTROL_ADDRESS = 0x0000;
	constexpr u16 CHR0_ADDRESS    = 0x2000;
	constexpr u16 CHR1_ADDRESS    = 0x4000;
	constexpr u16 PRG_ADDRESS     = 0x6000;

	u16 maskedAddress = address & ADDRESS_MASK;

	switch (maskedAddress)
	{
		case CONTROL_ADDRESS:
		{	
			u8 control = mCpuShiftRegister;

			// Set nametable arrangement
			mChrBankMode = (control & 0b1'0000) >> 4;
			mPrgBankMode = (control & 0b0'1100) >> 2;
			u8 ntBits    = (control & 0b0'0011);

			switch (ntBits)
			{
				case 0:
					mVramBankAddressOffset = 0;
					mNtArrangement = NametableArrangement::ONE_SCREEN;
					break;

				case 1:
					mVramBankAddressOffset = 0b0000'0100'0000'0000;
					mNtArrangement = NametableArrangement::ONE_SCREEN;
					break;
				
				case 2:
					mNtArrangement = NametableArrangement::HOR;
					break;

				
				case 3:
					mNtArrangement = NametableArrangement::VERT;
					break;
			}
		} break;
			
		case CHR0_ADDRESS:
			mChrBank0Idx = mCpuShiftRegister;
			break;
			
		case CHR1_ADDRESS:
			mChrBank1Idx = mCpuShiftRegister;
			break;
			
		case PRG_ADDRESS:
			// TODO: bit 4
			mPrgBankIdx = mCpuShiftRegister & 0b1111;
			break;
	}

	// Reset shift register
	resetShiftRegister();
}

bool Mapper001::mapPpuAddress(u16 address, u32 &mappedAddress)
{
	// Mapping for CHR-ROM/RAM (pattern tables)
	if (address <= 0x1FFF)
	{
		// Map address
		switch (mChrBankMode)
		{
			case 0:
			{
				u32 offset = (mChrBank0Idx & 0b1'1110) << 12;
				mappedAddress = offset | address;
			} break;
				
			case 1:
			{
				u32 offset = address < 0x1000 ? 
				             (mChrBank0Idx & 0b1'1111) << 12 :
				             (mChrBank1Idx & 0b1'1111) << 12;
				mappedAddress = offset | (address & 0x0FFF);
			} break;
		}

		return true;
	}

	// The remainder of the memory is in the PPU
	return false;
}
