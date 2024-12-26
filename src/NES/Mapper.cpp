#include "NES/Mapper.hpp"

Mapper::Mapper(u8 prgNumBanks, u8 chrNumBanks, NametableArrangement ntArr)
	: mPrgNumBanks(prgNumBanks), mChrNumBanks(chrNumBanks), mNtArrangement(ntArr)
{
	mVramBankAddressOffset = 0;

	mIsChrRamSelected = false;
	mIsPrgRamRead = false;

	mIsIrqSignalSet = false;
}