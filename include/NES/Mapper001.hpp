#pragma once

#include "NES/Mapper.hpp"

class Mapper001 : public Mapper
{
public:
	Mapper001(u8 prgNumBanks, u8 chrNumBanks);
	
	void reset() override;
	void resetShiftRegister();
	
	bool mapCpuWrite(u16 address, u32& mappedAddress, u8 value) override;
	bool mapCpuRead(u16 address, u32& mappedAddress) override;
	bool mapPpuWrite(u16 address, u32& mappedAddress) override;
	bool mapPpuRead(u16 address, u32& mappedAddress) override;
	
private:
	void processShiftRegister(u16 address);
	bool mapPpuAddress(u16 address, u32& mappedAddress);

	static constexpr u8 SHIFT_REGISTER_SIZE = 5;

	// Shift register
	u8 mCpuShiftRegister;
	u8 mCpuShiftCount;

	// Control register
	u8 mPrgBankMode;
	u8 mChrBankMode;

	// Chr0 register
	u8 mChrBank0Idx;

	// Chr1 register
	u8 mChrBank1Idx;

	// Prg register
	u8 mPrgBankIdx;
};
