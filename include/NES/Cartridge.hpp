#pragma once

#include <string>
#include <vector>
#include <memory>

#include "NES/Config.hpp"
#include "NES/Mapper.hpp"
#include "NES/Mapper000.hpp"

class Cartridge
{
public:
	Cartridge(const std::string& romFilename);

	void reset();

	bool readPrg(word cpuAddress, byte& output);
	bool writePrg(word cpuAddress, byte input);

private:
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

	void printHeaderInfo(bool isINesHeader, 
                         uint32_t prgRomSize,
	                     uint32_t chrRomSize,
						 NametableArrangement ntArrangement,
						 bool hasPrgRam,
						 bool hasTrainer,
						 bool hasAltNtLayout,
						 byte mapperNum,
						 bool isVsUnisystem,
						 bool isPlaychoice10,
						 bool isNes2Header,
						 uint32_t prgRamSize,
						 TVSystem tvSystem);

	std::vector<byte> mPrgRom;
	std::vector<byte> mPrgRam;
	std::vector<byte> mChrRom;

	std::shared_ptr<Mapper> mMapper;
};
