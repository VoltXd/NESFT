#pragma once

#include "Config.hpp"

class Mapper
{
public:
	Mapper(byte prgNumBanks, byte chrNumBanks);

	virtual void reset() = 0;

	virtual bool mapCpuWrite(word address, uint32_t& mappedAddress) = 0;
	virtual bool mapCpuRead(word address, uint32_t& mappedAddress) = 0;
	virtual bool mapPpuWrite(word address, uint32_t& mappedAddress) = 0;
	virtual bool mapPpuRead(word address, uint32_t& mappedAddress) = 0;

protected:
	byte mPrgNumBanks;
	byte mChrNumBanks;
};
