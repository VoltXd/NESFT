#pragma once

#include "NES/Mapper.hpp"

class Mapper000 : public Mapper
{
public:
	Mapper000(u8 prgNumBanks, u8 chrNumBanks) : Mapper(prgNumBanks, chrNumBanks) {}

	void reset() override;
	
	bool mapCpuWrite(u16 address, uint32_t& mappedAddress) override;
	bool mapCpuRead(u16 address, uint32_t& mappedAddress) override;
	bool mapPpuWrite(u16 address, uint32_t& mappedAddress) override;
	bool mapPpuRead(u16 address, uint32_t& mappedAddress) override;
	
private:
};
