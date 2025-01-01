#pragma once

#include <array>
#include "NES/Mapper.hpp"
#include "NES/Divider.hpp"

class Mapper004 : public Mapper
{
public:
	Mapper004(u8 prgNumBanks, u8 chrNumBanks, NametableArrangement ntArr);

	void reset() override;

	bool mapCpuWrite(u16 address, u32& mappedAddress, u8 value) override;
	bool mapCpuRead(u16 address, u32& mappedAddress) override;
	bool mapPpuWrite(u16 address, u32& mappedAddress, u16 ppuCycleCount) override;
	bool mapPpuRead(u16 address, u32& mappedAddress, u16 ppuCycleCount) override;

private:
	void processRegisterWrite(u16 address, u8 value);
	bool mapPpuAddress(u16 address, u32& mappedAddress, u16 ppuCycleCount);
	void processIrqCounter(u16 address, u16 ppuCycleCount);
	void clockIrqCounter();

	void irqLog(u16 cycle, u16 address, bool hasClocked);

	// Bank select register
	static constexpr u8 NUM_BANK_REGISTERS = 0b0000'1000;
	u8 mBankSelected;
	u8 mPrgBankMode;
	u8 mChrBankMode;

	// Bank registers
	std::array<u8, NUM_BANK_REGISTERS> mBankRegisters = {{ 0 }}; 

	// IRQ
	Divider mIrqCounter;
	u16 mPreviousCounter;
	u8 mPreviousA12;
	u16 mPpuCycleElapsed;
	u16 mPreviousPpuCycle;
	u8 mM2CycleOffset;
	bool mIsIrqReloadSet;
	bool mIsIrqEnabled;
};
