#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <memory>

#include "NES/Config.hpp"
#include "NES/Mapper.hpp"

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

	bool readChr(u16 ppuAddress, u8& output, u16& mappedNtAddress);
	bool writeChr(u16 ppuAddress, u8 input, u16& mappedNtAddress);

private:
	u16 mapNtAddress(u16 ppuAddress);
	void savePrgRam();
	std::string getHeaderInfo(bool isINesHeader, 
                              u32 prgRomSize,
	                          u32 chrRomSize,
					          NametableArrangement ntArrangement,
					          bool hasPrgRam,
					          bool hasTrainer,
					          bool hasAltNtLayout,
					          u8 mapperNum,
					          bool isVsUnisystem,
					          bool isPlaychoice10,
					          bool isNes2Header,
					          u32 prgRamSize,
					          TVSystem tvSystem);

	std::string mHeaderInfo;

	std::vector<u8> mPrgRom;
	std::vector<u8> mPrgRam;
	std::vector<u8> mChrRom;
	std::vector<u8> mChrRam;

	std::ofstream mCartRamFile;

	std::unique_ptr<Mapper> mMapper;
};
