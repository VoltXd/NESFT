#pragma once

#include "NES/Config.hpp"

class Mapper
{
public:
	Mapper(u8 prgNumBanks, u8 chrNumBanks);

	virtual void reset() = 0;

	virtual bool mapCpuWrite(u16 address, uint32_t& mappedAddress) = 0;
	virtual bool mapCpuRead(u16 address, uint32_t& mappedAddress) = 0;
	virtual bool mapPpuWrite(u16 address, uint32_t& mappedAddress) = 0;
	virtual bool mapPpuRead(u16 address, uint32_t& mappedAddress) = 0;

protected:
	u8 mPrgNumBanks;
	u8 mChrNumBanks;
};
