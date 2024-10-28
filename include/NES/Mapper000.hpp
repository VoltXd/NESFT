#pragma once

#include "NES/Mapper.hpp"

class Mapper000 : public Mapper
{
public:
	Mapper000(byte prgNumBanks, byte chrNumBanks) : Mapper(prgNumBanks, chrNumBanks) {}

	void reset() override;
	
	bool mapCpuWrite(word address, uint32_t& mappedAddress) override;
	bool mapCpuRead(word address, uint32_t& mappedAddress) override;
	bool mapPpuWrite(word address, uint32_t& mappedAddress) override;
	bool mapPpuRead(word address, uint32_t& mappedAddress) override;
	
private:
};
