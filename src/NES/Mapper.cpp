#include "NES/Mapper.hpp"

Mapper::Mapper(u8 prgNumBanks, u8 chrNumBanks)
{
	mPrgNumBanks = prgNumBanks;
	mChrNumBanks = chrNumBanks;
}