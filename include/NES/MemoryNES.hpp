#pragma once

#include <array>
#include <string>
#include <memory>
#include "NES/Config.hpp"
#include "NES/Cartridge.hpp"
#include "NES/Controller.hpp"

constexpr u16 PPUCTRL_CPU_ADDR   = 0x2000;
constexpr u16 PPUMASK_CPU_ADDR   = 0x2001;
constexpr u16 PPUSTATUS_CPU_ADDR = 0x2002;
constexpr u16 OAMADDR_CPU_ADDR   = 0x2003;
constexpr u16 OAMDATA_CPU_ADDR   = 0x2004;
constexpr u16 PPUSCROLL_CPU_ADDR = 0x2005;
constexpr u16 PPUADDR_CPU_ADDR   = 0x2006;
constexpr u16 PPUDATA_CPU_ADDR   = 0x2007;

class PPU;
class APU;

/// @brief Memory class for 6502 CPU Tests (RAM is 64 KB, while NES CPU RAM is 2 KB)
class MemoryNES
{
public:
	MemoryNES(const std::string& romFilename, APU& apuRef, PPU& ppuRef, Controller& controllerRef);
	void reset();

	u8 cpuRead(u16 address);
	void cpuWrite(u16 address, u8 value);
	
	u8 ppuRead(u16 address, u16 ppuCycleCount);
	void ppuWrite(u16 address, u8 value, u16 ppuCycleCount);

	inline bool isOamDmaStarted() const { return mIsOamDmaStarted; }
	s32 executeOamDma(bool isGetCycle);

	inline bool getCartridgeIrq() const { return mCartridge.getIrqSignal(); }
	inline void clearCartridgeIrq() { mCartridge.clearIrqSignal(); }

private:
	// DMA
	void startOamDma(u8 pageAddress);

private:
	// CPU
	static constexpr u32 CPU_RAM_SIZE = 0x0800; // 2 kB
	std::array<u8, CPU_RAM_SIZE> mCpuRam;

	// APU
	APU& mApuRef;

	// PPU
	static constexpr u32 PPU_VRAM_SIZE = 0x0800; // 2 kB
	std::array<u8, PPU_VRAM_SIZE> mPpuVram;

	PPU& mPpuRef;

	// Cartridge
	Cartridge mCartridge;

	// DMA
    static constexpr u16 OAMDMA_CPU_ADDR = 0x4014;
	u8 mOamDma;
	u8 mOamDmaBuffer;
	u16 mOamDmaIdx;
	bool mIsOamDmaStarted;
	bool mIsCpuHalt;
	bool mPreviousIsGetCycle;

	// Controller
	static constexpr u16 CONTROLLER_STROBE_ADDR = 0x4016;
	static constexpr u16 CONTROLLER_1_STATE_ADDR = 0x4016;
	Controller& mControllerRef;
};
