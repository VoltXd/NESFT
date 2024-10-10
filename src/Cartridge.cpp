#include "Cartridge.hpp"

#include <iostream>
#include <fstream>
#include "Config.hpp"
#include "Toolbox.hpp"

Cartridge::Cartridge(const std::string& romFilename)
{
	// ******** Header structure (iNES) ******** //
	struct
	{
		char identification[4];
		char prgNumBanks;
		char chrNumBanks;
		char flag6;
		char flag7;
		char flag8;
		char flag9;
		char flag10;
	} header;

	// Open file 
	std::ifstream romFile(romFilename, std::ios::binary);
	
	// ******** Read header ******** //
	romFile.read(header.identification, 4);
	romFile.read(&header.prgNumBanks, 1);
	romFile.read(&header.chrNumBanks, 1);
	romFile.read(&header.flag6, 1);
	romFile.read(&header.flag7, 1);
	romFile.read(&header.flag8, 1);
	romFile.read(&header.flag9, 1);
	romFile.read(&header.flag10, 1);
	romFile.seekg(5, std::ios_base::cur);

	// Translate header
	bool isINesHeader = false;
	if (header.identification[0] == 'N' &&
	    header.identification[1] == 'E' &&
	    header.identification[2] == 'S' &&
	    header.identification[3] == '\x1A')
		isINesHeader = true;

	uint32_t prgRomSize = header.prgNumBanks << 14; // 16 kB unit
	uint32_t chrRomSize = header.chrNumBanks << 13; //  8 kB unit 

	// Flag 6
	NametableArrangement ntArrangement = (NametableArrangement)(header.flag6 & 0b0000'0001);
	bool hasPrgRam = (header.flag6 & 0b0000'0010) != 0;
	bool hasTrainer = (header.flag6 & 0b0000'0100) != 0;
	bool hasAltNtLayout = (header.flag6 & 0b0000'1000) != 0;
	byte mapperNum = (header.flag6 & 0xF0) >> 4;

	// Flag 7
	bool isVsUnisystem = (header.flag7 & 0b0000'0001) != 0;
	bool isPlaychoice10 = (header.flag7 & 0b0000'0010) != 0;
	bool isNes2Header = ((header.flag7 & 0b0000'1100) >> 2) == 2;
	mapperNum |= (header.flag7 & 0xF0);

	// Flag 8
	uint32_t prgRamSize = header.flag8 << 13; // 8 kB unit

	// Flag 9
	TVSystem tvSystem = (TVSystem)(header.flag9 & 0b0000'0001);

	// Flag 10 (redundant)
	// Print ROM info
	printHeaderInfo(isINesHeader, prgRomSize, chrRomSize, ntArrangement, hasPrgRam, hasTrainer,
	                hasAltNtLayout, mapperNum, isVsUnisystem, isPlaychoice10, isNes2Header, 
					prgRamSize, tvSystem);

	// Crash the program on unwanted header
	testAndExitWithMessage(!isINesHeader, "ROM file is not iNES.");
	testAndExitWithMessage(hasPrgRam, "PRG-RAM not implemented.");
	testAndExitWithMessage(hasTrainer, "Trainer not implemented.");
	testAndExitWithMessage(hasAltNtLayout, "Alternative Nametable Layout not implemented.");
	testAndExitWithMessage(isVsUnisystem, "VS Unisystem not implemented.");
	testAndExitWithMessage(isPlaychoice10, "PlayChoice-10 not implemented.");
	testAndExitWithMessage(isNes2Header, "NES 2.0 not implemented.");

	// Create a mapper object
	switch (mapperNum)
	{
		case 0:
			mMapper = std::make_shared<Mapper000>(header.prgNumBanks, header.chrNumBanks);
			break;
		
		default:
			testAndExitWithMessage(false, "Mappers other than 0 not implemented.");
			break;
	}

	// ******** Read trainer (if present (Not implemented)) ******** //
	// ******** Read PRG-ROM ******** //
	mPrgRom.resize(prgRomSize);
	romFile.read((char*)mPrgRom.data(), prgRomSize);

	// ******** Read CHR-ROM (if present) ******** //
	testAndExitWithMessage(chrRomSize == 0, "CHR-RAM not implemented.");
	mChrRom.resize(chrRomSize);
	romFile.read((char*)mChrRom.data(), chrRomSize);

	// ******** Read PlayChoice INST-ROM (if present (WTF????? (Not implemented))) ******** //
	// ******** Read PlayChoice PROM (if present (WTF????? (Not Implemented))) ******** //
}

void Cartridge::reset()
{
	if (mMapper != nullptr)
		mMapper->reset();
}

bool Cartridge::readPrg(word cpuAddress, byte &output)
{
	uint32_t prgRomAddr;
	if (!mMapper->mapCpuRead(cpuAddress, prgRomAddr))
		return false;

	output = mPrgRom[prgRomAddr];
	return true;
}

bool Cartridge::writePrg(word cpuAddress, byte input)
{
	// Write into the PRG-RAM
	uint32_t prgRamAddr;
	if (!mMapper->mapCpuWrite(cpuAddress, prgRamAddr))
		return false;

	mPrgRam[prgRamAddr] = input;
	return true;
}

void Cartridge::printHeaderInfo(bool isINesHeader, 
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
                                TVSystem tvSystem)
{
	std::cout << "iNES Header found: "     << std::boolalpha << isINesHeader << std::dec << '\n'
	          << "PRG-ROM size: "          << prgRomSize << '\n'
	          << "CHR-ROM size: "          << chrRomSize << '\n'
	          << "PRG-RAM present: "       << hasPrgRam << '\n'
	          << "PRG-RAM size: "          << prgRamSize << '\n'
	          << "Trainer present: "       << hasTrainer << '\n'
	          << "Nametable Layout: "      << ((ntArrangement == NametableArrangement::VERT) ? "Vertical" : "Horizontal") << '\n'
	          << "Alt. Nametable Layout: " << hasAltNtLayout << '\n'
	          << "Mapper: "                << +mapperNum << '\n'
	          << "VS UniSystem: "         << isVsUnisystem << '\n'
	          << "PlayChoice-10: "         << isPlaychoice10 << '\n'
	          << "NES 2.0 format: "        << isNes2Header << '\n'
	          << "TV System: "             << ((tvSystem == TVSystem::NTFS) ? "NTFS" : "PAL") << '\n'
			  << std::endl;
}
