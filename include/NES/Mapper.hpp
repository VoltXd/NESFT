#pragma once

#include "NES/Config.hpp"

enum NametableArrangement
{
	VERT = 0,
	HOR = 1,
	ONE_SCREEN = 2,
	FOUR_SCREEN = 3
};

class Mapper
{
public:
	Mapper(u8 prgNumBanks, u8 chrNumBanks, NametableArrangement ntArr);
	virtual ~Mapper() {}

	virtual void reset() = 0;

	virtual bool mapCpuWrite(u16 address, u32& mappedAddress, u8 value) = 0;
	virtual bool mapCpuRead(u16 address, u32& mappedAddress) = 0;
	virtual bool mapPpuWrite(u16 address, u32& mappedAddress, u16 ppuCycleCount) = 0;
	virtual bool mapPpuRead(u16 address, u32& mappedAddress, u16 ppuCycleCount) = 0;

	inline NametableArrangement getNtArragenement() const { return mNtArrangement; }
	inline u16 getVramBankAddressOffset() const { return mVramBankAddressOffset; }
	inline bool isPrgRamRead() const { return mIsPrgRamRead; };
	inline bool isChrRamSelected() const { return mIsChrRamSelected; };

	inline bool getIrqSignal() const { return mIsIrqSignalSet; }
	inline void clearIrqSignal() { mIsIrqSignalSet = false; }

protected:
	const u8 mPrgNumBanks;
	const u8 mChrNumBanks;
	
	NametableArrangement mNtArrangement;
	u16 mVramBankAddressOffset;
	
	bool mIsPrgRamRead;
	bool mIsChrRamSelected;

	bool mIsIrqSignalSet;
};
