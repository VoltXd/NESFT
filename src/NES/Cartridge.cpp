#include "NES/Cartridge.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "NES/Config.hpp"
#include "NES/Mapper000.hpp"
#include "NES/Mapper001.hpp"
#include "NES/Mapper002.hpp"
#include "NES/Toolbox.hpp"

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
	std::filesystem::path romPath = romFilename;
	testAndExitWithMessage(romPath.extension() != ".nes", "Only .nes files are accepted !");

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

	u32 prgRomSize = header.prgNumBanks << 14; // 16 kB unit
	u32 chrRomSize = header.chrNumBanks << 13; //  8 kB unit 

	// Flag 6
	NametableArrangement ntArr = (NametableArrangement)(header.flag6 & 0b0000'0001);
	bool hasBatteryPrgRam = (header.flag6 & 0b0000'0010) != 0;
	bool hasTrainer = (header.flag6 & 0b0000'0100) != 0;
	bool hasAltNtLayout = (header.flag6 & 0b0000'1000) != 0;
	u8 mapperNum = (header.flag6 & 0xF0) >> 4;

	// Flag 7
	bool isVsUnisystem = (header.flag7 & 0b0000'0001) != 0;
	bool isPlaychoice10 = (header.flag7 & 0b0000'0010) != 0;
	bool isNes2Header = ((header.flag7 & 0b0000'1100) >> 2) == 2;
	mapperNum |= (header.flag7 & 0xF0);

	// Flag 8
	u32 prgRamSize = header.flag8 << 13; // 8 kB unit

	// Flag 9
	TVSystem tvSystem = (TVSystem)(header.flag9 & 0b0000'0001);

	// Flag 10 (redundant)
	// Print ROM info
	mHeaderInfo = getHeaderInfo(isINesHeader, prgRomSize, chrRomSize, ntArr, hasBatteryPrgRam, hasTrainer,
	                            hasAltNtLayout, mapperNum, isVsUnisystem, isPlaychoice10, isNes2Header, 
					            prgRamSize, tvSystem);
	std::cout << mHeaderInfo;

	// Crash the program on unwanted header
	testAndExitWithMessage(!isINesHeader, "ROM file is not iNES.");
	testAndExitWithMessage(hasTrainer, "Trainer not implemented.");
	testAndExitWithMessage(hasAltNtLayout, "Alternative Nametable Layout not implemented.");
	testAndExitWithMessage(isVsUnisystem, "VS Unisystem not implemented.");
	testAndExitWithMessage(isPlaychoice10, "PlayChoice-10 not implemented.");
	testAndExitWithMessage(isNes2Header, "NES 2.0 not implemented.");

	// Create a mapper object
	u32 chrRamSize = 0;
	switch (mapperNum)
	{
		case 0:
			mMapper = std::make_unique<Mapper000>(header.prgNumBanks, header.chrNumBanks, ntArr);
			break;

		case 1:
			mMapper = std::make_unique<Mapper001>(header.prgNumBanks, header.chrNumBanks);

			// Assume CHR-RAM 8 KB
			chrRamSize = 0x2000;
			
			// Assume PRG-RAM 8 KB
			mPrgRam.resize(0x2000);
			break;

		case 2:
			mMapper = std::make_unique<Mapper002>(header.prgNumBanks, header.chrNumBanks, ntArr);
			chrRamSize = 0x2000;
			break;
		
		default:
			testAndExitWithMessage(false, "Mappers other than 0 not implemented.");
			break;
	}

	// Initialise save PRG-RAM file
	if (hasBatteryPrgRam)
	{
		std::string saveFilename = romFilename + "save";
		if (std::filesystem::exists(saveFilename))
		{
			// A save has been found, dump to PRG-RAM
			std::ifstream saveFile(saveFilename, std::ios::binary);
			saveFile.read((char*)mPrgRam.data(), mPrgRam.size());
		}
		else
		{
			// Save file not found, create it
			std::ofstream saveFile(saveFilename, std::ios::binary);
			saveFile.write((char*)mPrgRam.data(), mPrgRam.size());
		}

		// Open save file in R/W mode
		mCartRamFile = std::ofstream(saveFilename, std::ios::binary);
	}

	// ******** Read trainer (if present (Not implemented)) ******** //
	// ******** Read PRG-ROM ******** //
	mPrgRom.resize(prgRomSize);
	romFile.read((char*)mPrgRom.data(), prgRomSize);

	// ******** Read CHR-ROM (if present) ******** //
	if (chrRomSize != 0)
	{
		// CHR-ROM is present
		mChrRom.resize(chrRomSize);
		romFile.read((char*)mChrRom.data(), chrRomSize);
	}
	else
	{
		// No CHR-ROM => CHR-RAM is present (some mappers have both
		// but only for 2 games -> will likely not be implemented)
		mChrRam.resize(chrRamSize);
	}

	// ******** Read PlayChoice INST-ROM (if present (WTF????? (Not implemented))) ******** //
	// ******** Read PlayChoice PROM (if present (WTF????? (Not Implemented))) ******** //
}

void Cartridge::reset()
{
	if (mMapper != nullptr)
		mMapper->reset();
	
	savePrgRam();
}

bool Cartridge::readPrg(u16 cpuAddress, u8 &output)
{
	u32 prgAddr;
	if (!mMapper->mapCpuRead(cpuAddress, prgAddr))
		return false;

	// Check if target is PRG RAM or ROM
	output = mMapper->isPrgRamRead() ?
	         mPrgRam[prgAddr] :
			 mPrgRom[prgAddr];

	return true;
}

bool Cartridge::writePrg(u16 cpuAddress, u8 input)
{
	// Write into the PRG-RAM
	u32 prgRamAddr;
	if (!mMapper->mapCpuWrite(cpuAddress, prgRamAddr, input))
		return false;

	mPrgRam[prgRamAddr] = input;
	return true;
}

bool Cartridge::readChr(u16 ppuAddress, u8& output, u16& mappedNtAddress)
{
	u32 chrAddr;
	if (!mMapper->mapPpuRead(ppuAddress, chrAddr))
	{
		mappedNtAddress = mapNtAddress(ppuAddress);
		return false;
	}

	output = mMapper->isChrRamSelected() ?
	         mChrRam[chrAddr] :
			 mChrRom[chrAddr];
	return true;
}

bool Cartridge::writeChr(u16 ppuAddress, u8 input, u16& mappedNtAddress)
{
	u32 chrRamAddr;
	if (!mMapper->mapPpuWrite(ppuAddress, chrRamAddr))
	{
		mappedNtAddress = mapNtAddress(ppuAddress);
		return false;
	}

	if (mMapper->isChrRamSelected())
	{
		mChrRam[chrRamAddr] = input;
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "Write to CHR-RAM not implemented: " << __FILE__ << ":" << __LINE__;
		testAndExitWithMessage(true, errorMessage.str());
	}

    return true;
}

u16 Cartridge::mapNtAddress(u16 ppuAddress)
{
	u16 mappedNtAddress;
	NametableArrangement ntArr = mMapper->getNtArragenement();

	// Map address to VRAM address (Nametable)
	if (ntArr == NametableArrangement::VERT)
		mappedNtAddress = (ppuAddress & 0b0000'0011'1111'1111) | ((ppuAddress >> 1) & 0b0000'0100'0000'0000);
	else if (ntArr == NametableArrangement::HOR)
		mappedNtAddress = ppuAddress & 0b0000'0111'1111'1111;
	else
		mappedNtAddress = ppuAddress & 0b0000'0011'1111'1111 + mMapper->getVramBankAddressOffset();

	return mappedNtAddress;
}

inline void Cartridge::savePrgRam()
{
	// Save only when a battery-backed RAM cartridge is present
	if (mCartRamFile.is_open())
	{
		mCartRamFile.write((char*)mPrgRam.data(), mPrgRam.size());	
		mCartRamFile.seekp(0);
	}
}

inline std::string Cartridge::getHeaderInfo(bool isINesHeader, 
                                            u32 prgRomSize, 
                                            u32 chrRomSize, 
								            NametableArrangement ntArrangement,
                                            bool hasBatteryPrgRam, 
								            bool hasTrainer, 
								            bool hasAltNtLayout, 
								            u8 mapperNum,
                                            bool isVsUnisystem, 
								            bool isPlaychoice10, 
								            bool isNes2Header, 
								            u32 prgRamSize,
                                            TVSystem tvSystem)
{
	std::stringstream headerInfo;
	headerInfo << "iNES Header found: "          << std::boolalpha << isINesHeader << std::dec << '\n'
	           << "PRG-ROM size: "               << prgRomSize << '\n'
	           << "CHR-ROM size: "               << chrRomSize << '\n'
	           << "Battery backed RAM present: " << hasBatteryPrgRam << '\n'
	           << "PRG-RAM size: "               << prgRamSize << '\n'
	           << "Trainer present: "            << hasTrainer << '\n'
	           << "Nametable Layout: "           << ((ntArrangement == NametableArrangement::VERT) ? "Vertical" : "Horizontal") << '\n'
	           << "Alt. Nametable Layout: "      << hasAltNtLayout << '\n'
	           << "Mapper: "                     << +mapperNum << '\n'
	           << "VS UniSystem: "               << isVsUnisystem << '\n'
	           << "PlayChoice-10: "              << isPlaychoice10 << '\n'
	           << "NES 2.0 format: "             << isNes2Header << '\n'
	           << "TV System: "                  << ((tvSystem == TVSystem::NTFS) ? "NTFS" : "PAL") << '\n'
			   << std::endl;
	
	return headerInfo.str();
}
