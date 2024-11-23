#include "NES/APU.hpp"

#include <sstream>
#include "NES/Toolbox.hpp"

APU::APU()
	: mPulse1Channel(true),
	  mPulse2Channel(false)
{
}

void APU::reset()
{
	mPulse1Reg.reg0 = 0x00;
	mPulse1Reg.reg1 = 0x00;
	mPulse1Reg.reg2 = 0x00;
	mPulse1Reg.reg3 = 0x00;

	mPulse2Reg.reg0 = 0x00;
	mPulse2Reg.reg1 = 0x00;
	mPulse2Reg.reg2 = 0x00;
	mPulse2Reg.reg3 = 0x00;

	mTriangleReg.reg0 = 0x00;
	mTriangleReg.reg1 = 0x00;
	mTriangleReg.reg2 = 0x00;

	mNoiseReg.reg0 = 0x00;
	mNoiseReg.reg1 = 0x00;
	mNoiseReg.reg2 = 0x00;
	
	mDmcReg.reg0 = 0x00;
	mDmcReg.reg1 = 0x00;
	mDmcReg.reg2 = 0x00;
	mDmcReg.reg3 = 0x00;

	mStatus = 0x00;

	mFrameCounter.reset();
	mPulse1Channel.reset();
	mPulse2Channel.reset();
	mTriangleChannel.reset();
	mNoiseChannel.reset();
	mDmcChannel.reset();
}

s32 APU::executeOneCpuCycle(Memory& memory, bool isGetCycle)
{
	APUFrameCounterState fcState;
	s32 extraCycles = 0;

	// Frame counter
	fcState = mFrameCounter.executeOneCpuCycle();

	// On every tick -> tick triangle
	mTriangleChannel.update(fcState);

	if (!mFrameCounter.isEvenCycle())
	{
		// On even cycle -> tick pulse, noise & DMC
		mPulse1Channel.update(fcState);
		mPulse2Channel.update(fcState);
		mNoiseChannel.update(fcState);
		extraCycles = mDmcChannel.update(memory, isGetCycle);
	}

	return extraCycles;
}

float APU::getOutput()
{
	u8 pulse1 = mPulse1Channel.getOutput();
	u8 pulse2 = mPulse2Channel.getOutput();
	u8 triangle = mTriangleChannel.getOutput();
	u8 noise = mNoiseChannel.getOutput();
	u8 dmc = mDmcChannel.getOutput();
	float output;

	// Store the APU mixed output
	output = mix(pulse1, pulse2, triangle, noise, dmc);

	return output;
}

float APU::getPulse1Output()
{
	u8 pulse1 = mPulse1Channel.getOutput();

    return mixPulses(pulse1, 0);
}

float APU::getPulse2Output()
{
	u8 pulse2 = mPulse2Channel.getOutput();

    return mixPulses(pulse2, 0);
}

float APU::getTriangleOutput()
{
    u8 triangle = mTriangleChannel.getOutput();

	return mixTnd(triangle, 0, 0);
}

float APU::getNoiseOutput()
{   
    u8 noise = mNoiseChannel.getOutput();

	return mixTnd(0, noise, 0);
}

float APU::getDMCOutput()
{
    u8 dmc = mDmcChannel.getOutput();

	return mixTnd(0, 0, dmc);
}

void APU::writeRegister(u16 address, u8 value)
{
	switch (address)
	{
		case APU_PULSE1_0_CPU_ADDR:
			mPulse1Reg.reg0 = value;
			mPulse1Channel.setReg0(value);
			break;
			
		case APU_PULSE1_1_CPU_ADDR:
			mPulse1Reg.reg1 = value;
			mPulse1Channel.setReg1(value);
			break;
			
		case APU_PULSE1_2_CPU_ADDR:
			mPulse1Reg.reg2 = value;
			mPulse1Channel.setReg2(value);
			break;
			
		case APU_PULSE1_3_CPU_ADDR:
			mPulse1Reg.reg3 = value;
			mPulse1Channel.setReg3(value);
			break;
			
		case APU_PULSE2_0_CPU_ADDR:
			mPulse2Reg.reg0 = value;
			mPulse2Channel.setReg0(value);
			break;
			
		case APU_PULSE2_1_CPU_ADDR:
			mPulse2Reg.reg1 = value;
			mPulse2Channel.setReg1(value);
			break;
			
		case APU_PULSE2_2_CPU_ADDR:
			mPulse2Reg.reg2 = value;
			mPulse2Channel.setReg2(value);
			break;
			
		case APU_PULSE2_3_CPU_ADDR:
			mPulse2Reg.reg3 = value;
			mPulse2Channel.setReg3(value);
			break;
			
		case APU_TRIANGLE_0_CPU_ADDR:
			mTriangleReg.reg0 = value;
			mTriangleChannel.setReg0(value);
			break;
			
		case APU_TRIANGLE_1_CPU_ADDR:
			// Unused register
			break;
			
		case APU_TRIANGLE_2_CPU_ADDR:
			mTriangleReg.reg1 = value;
			mTriangleChannel.setReg1(value);
			break;
			
		case APU_TRIANGLE_3_CPU_ADDR:
			mTriangleReg.reg2 = value;
			mTriangleChannel.setReg2(value);
			break;
			
		case APU_NOISE_0_CPU_ADDR:
			mNoiseReg.reg0 = value;
			mNoiseChannel.setReg0(value);
			break;
			
		case APU_NOISE_1_CPU_ADDR:
			// Unused register
			break;
			
		case APU_NOISE_2_CPU_ADDR:
			mNoiseReg.reg1 = value;
			mNoiseChannel.setReg1(value);
			break;
			
		case APU_NOISE_3_CPU_ADDR:
			mNoiseReg.reg2 = value;
			mNoiseChannel.setReg2(value);
			break;
			
		case APU_DMC_0_CPU_ADDR:
			mDmcReg.reg0 = value;
			mDmcChannel.setReg0(value);
			break;
			
		case APU_DMC_1_CPU_ADDR:
			mDmcReg.reg1 = value;
			mDmcChannel.setReg1(value);
			break;
			
		case APU_DMC_2_CPU_ADDR:
			mDmcReg.reg2 = value;
			mDmcChannel.setReg2(value);
			break;
			
		case APU_DMC_3_CPU_ADDR:
			mDmcReg.reg3 = value;
			mDmcChannel.setReg3(value);
			break;
		
		case APU_STATUS_CPU_ADDR:
			mStatus = value;
			// Pulse 1
			if ((mStatus & 0b0000'0001) == 0)
				mPulse1Channel.disable();
			else
				mPulse1Channel.enable();

			// Pulse 2
			if ((mStatus & 0b0000'0010) == 0)
				mPulse2Channel.disable();
			else
				mPulse2Channel.enable();

			// triangle
			if ((mStatus & 0b0000'0100) == 0)
				mTriangleChannel.disable();
			else
				mTriangleChannel.enable();
			
			// Noise
			if ((mStatus & 0b0000'1000) == 0)
				mNoiseChannel.disable();
			else
				mNoiseChannel.enable();
			
			// DMC
			if ((mStatus & 0b0001'0000) == 0)
				mDmcChannel.disable();
			else
				mDmcChannel.enable();

			break;

		case APU_FRAME_COUNTER_CPU_ADDR:
			mFrameCounterReg = value;
			mFrameCounter.writeRegister(mFrameCounterReg);
			break;
		
		default:
			std::stringstream errorStream;
			errorStream << std::hex << "APU register 0x" << address << " is not writeable.";
			testAndExitWithMessage(true, errorStream.str());
			break;
	}
}

u8 APU::readRegister(u16 address)
{
	u8 value = 0;
	
	if (address == APU_STATUS_CPU_ADDR)
	{
		// TODO: status
		bool pulse1Status   = mPulse1Channel.getStatus();
		bool pulse2Status   = mPulse2Channel.getStatus();
		bool triangleStatus = mTriangleChannel.getStatus();
		bool noiseStatus    = mNoiseChannel.getStatus();
		bool dmcStatus      = mDmcChannel.getStatus();

		bool dmcIrq = mDmcChannel.getIRQSignal();
		bool fcIrq  = mFrameCounter.getIRQSignal();

		mStatus = (dmcIrq         ? (1 << 7) : 0) |
		          (fcIrq          ? (1 << 6) : 0) |
		          (dmcStatus      ? (1 << 4) : 0) |
		          (noiseStatus    ? (1 << 3) : 0) |
		          (triangleStatus ? (1 << 2) : 0) |
				  (pulse2Status   ? (1 << 1) : 0) |
				  (pulse1Status   ? (1 << 0) : 0);
		value = mStatus;
	}
	else 
	{
		std::stringstream errorStream;
		errorStream << std::hex << "APU register 0x" << address << " is not readable.";
		testAndExitWithMessage(true, errorStream.str());
	}
	
	return value;
}

float APU::mixPulses(u8 pulse1, u8 pulse2)
{
	float pulseOut;
	
	u8 pulse12 = pulse1 + pulse2;
	if (pulse12 == 0)
		pulseOut = 0.0f;
	else
		pulseOut = (95.88f * pulse12) / (8128 + 100.0f * pulse12);

    return pulseOut;
}

float APU::mixTnd(u8 triangle, u8 noise, u8 dmc)
{
	float tndTemp, tndOut;

	u8 tnd = triangle + noise + dmc;
	if (tnd == 0)
		tndOut = 0.0f;
	else
	{
		tndTemp = triangle * (1.0f / 8227)  + 
				  noise    * (1.0f / 12241) + 
				  dmc      * (1.0f / 22638);
		tndOut = (159.79f * tndTemp) / (1.0f + 100.0f * tndTemp);
	}

    return tndOut;
}

float APU::mix(u8 pulse1, u8 pulse2, u8 triangle, u8 noise, u8 dmc) 
{
	float pulseOut = mixPulses(pulse1, pulse2);	
	float tndOut = mixTnd(triangle, noise, dmc);
	
	return pulseOut + tndOut;
}
