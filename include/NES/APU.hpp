#pragma once

#include <array>

#include "NES/Config.hpp"
#include "NES/APUFrameCounter.hpp"
#include "NES/APUPulse.hpp"
#include "NES/APUTriangle.hpp"
#include "NES/APUNoise.hpp"

constexpr u16 APU_PULSE1_0_CPU_ADDR      = 0x4000;
constexpr u16 APU_PULSE1_1_CPU_ADDR      = 0x4001;
constexpr u16 APU_PULSE1_2_CPU_ADDR      = 0x4002;
constexpr u16 APU_PULSE1_3_CPU_ADDR      = 0x4003;

constexpr u16 APU_PULSE2_0_CPU_ADDR      = 0x4004;
constexpr u16 APU_PULSE2_1_CPU_ADDR      = 0x4005;
constexpr u16 APU_PULSE2_2_CPU_ADDR      = 0x4006;
constexpr u16 APU_PULSE2_3_CPU_ADDR      = 0x4007;

constexpr u16 APU_TRIANGLE_0_CPU_ADDR    = 0x4008;
constexpr u16 APU_TRIANGLE_1_CPU_ADDR    = 0x4009;
constexpr u16 APU_TRIANGLE_2_CPU_ADDR    = 0x400A;
constexpr u16 APU_TRIANGLE_3_CPU_ADDR    = 0x400B;

constexpr u16 APU_NOISE_0_CPU_ADDR       = 0x400C;
constexpr u16 APU_NOISE_1_CPU_ADDR       = 0x400D;
constexpr u16 APU_NOISE_2_CPU_ADDR       = 0x400E;
constexpr u16 APU_NOISE_3_CPU_ADDR       = 0x400F;

constexpr u16 APU_DMC_0_CPU_ADDR         = 0x4010;
constexpr u16 APU_DMC_1_CPU_ADDR         = 0x4011;
constexpr u16 APU_DMC_2_CPU_ADDR         = 0x4012;
constexpr u16 APU_DMC_3_CPU_ADDR         = 0x4013;

constexpr u16 APU_STATUS_CPU_ADDR        = 0x4015;
constexpr u16 APU_FRAME_COUNTER_CPU_ADDR = 0x4017;


class APU
{
public:
	APU();
	
	void reset();

	void executeOneCpuCycle();
	float getOutput();
	
	void writeRegister(u16 address, u8 value);
	u8 readRegister(u16 address);

	inline bool getFrameCounterIRQSignal() const { return mFrameCounter.getIRQSignal(); }
	inline void clearIRQSignal() { mFrameCounter.clearIRQSignal(); }

private:
	float mix(u8 pulse1, 
	          u8 pulse2,
			  u8 triangle,
			  u8 noise,
			  u8 dmc);

	// Registers
	struct pulseReg
	{
		u8 reg0;
		u8 reg1;
		u8 reg2;
		u8 reg3;
	} mPulse1Reg, mPulse2Reg;

	struct triangleReg
	{
		u8 reg0;
		u8 reg1;
		u8 reg2;
	} mTriangleReg;

	struct noiseReg
	{
		u8 reg0;
		u8 reg1;
		u8 reg2;
	} mNoiseReg;

	struct dmcReg
	{
		u8 reg0;
		u8 reg1;
		u8 reg2;
		u8 reg3;
	} mDmcReg;

	APUFrameCounter mFrameCounter;
	APUPulse mPulse1Channel;
	APUPulse mPulse2Channel;
	APUTriangle mTriangleChannel;
	APUNoise mNoiseChannel;

	u8 mStatus;
	u8 mFrameCounterReg;
};
