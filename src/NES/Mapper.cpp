#include "NES/Mapper.hpp"

Mapper::Mapper(byte prgNumBanks, byte chrNumBanks)
{
	mPrgNumBanks = prgNumBanks;
	mChrNumBanks = chrNumBanks;
}