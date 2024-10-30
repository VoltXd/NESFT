#pragma once

#include <string>
#include <vector>
#include <memory>

#include "NES/Config.hpp"
#include "NES/Mapper.hpp"
#include "NES/Mapper000.hpp"

enum NametableArrangement
{
	VERT = 0,
	HOR = 1,
};

enum TVSystem
{
	NTFS = 0,
	PAL = 1
};

class Cartridge
{
public:
	Cartridge(const std::string& romFilename);

	void reset();

	bool readPrg(u16 cpuAddress, u8& output);
	bool writePrg(u16 cpuAddress, u8 input);

	bool readChr(u16 ppuAddress, u8& output);
	bool writeChr(u16 ppuAddress, u8 input);

	inline NametableArrangement getNtArrangement() const { return mNtArrangement; }

private:
	void printHeaderInfo(bool isINesHeader, 
                         uint32_t prgRomSize,
	                     uint32_t chrRomSize,
						 NametableArrangement ntArrangement,
						 bool hasPrgRam,
						 bool hasTrainer,
						 bool hasAltNtLayout,
						 u8 mapperNum,
						 bool isVsUnisystem,
						 bool isPlaychoice10,
						 bool isNes2Header,
						 uint32_t prgRamSize,
						 TVSystem tvSystem);

	std::vector<u8> mPrgRom;
	std::vector<u8> mPrgRam;
	std::vector<u8> mChrRom;

	NametableArrangement mNtArrangement;

	std::shared_ptr<Mapper> mMapper;
};
