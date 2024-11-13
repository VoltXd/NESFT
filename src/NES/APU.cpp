#include "NES/APU.hpp"

#include <sstream>
#include "NES/Toolbox.hpp"

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
	mNoiseChannel.reset();
}

void APU::executeOneCpuCycle()
{
	APUFrameCounterState fcState;

	// Frame counter
	fcState = mFrameCounter.executeOneCpuCycle();

	if (!mFrameCounter.isEvenCycle())
	{
		// On even cycle -> tick pulse, noise & DMC
		mNoiseChannel.update(fcState);
	}
}

float APU::getOutput()
{
	u8 noise;
	float output;

	noise = mNoiseChannel.getOutput();

	// Store the APU mixed output
	output = mix(0, 0, 0, noise, 0);

	return output;
}

void APU::writeRegister(u16 address, u8 value)
{
	switch (address)
	{
		case APU_PULSE1_0_CPU_ADDR:
			mPulse1Reg.reg0 = value;
			break;
			
		case APU_PULSE1_1_CPU_ADDR:
			mPulse1Reg.reg1 = value;
			break;
			
		case APU_PULSE1_2_CPU_ADDR:
			mPulse1Reg.reg2 = value;
			break;
			
		case APU_PULSE1_3_CPU_ADDR:
			mPulse1Reg.reg3 = value;
			break;
			
		case APU_PULSE2_0_CPU_ADDR:
			mPulse2Reg.reg0 = value;
			break;
			
		case APU_PULSE2_1_CPU_ADDR:
			mPulse2Reg.reg1 = value;
			break;
			
		case APU_PULSE2_2_CPU_ADDR:
			mPulse2Reg.reg2 = value;
			break;
			
		case APU_PULSE2_3_CPU_ADDR:
			mPulse2Reg.reg3 = value;
			break;
			
		case APU_TRIANGLE_0_CPU_ADDR:
			mTriangleReg.reg0 = value;
			break;
			
		case APU_TRIANGLE_1_CPU_ADDR:
			// Unused register
			break;
			
		case APU_TRIANGLE_2_CPU_ADDR:
			mTriangleReg.reg1 = value;
			break;
			
		case APU_TRIANGLE_3_CPU_ADDR:
			mTriangleReg.reg2 = value;
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
			break;
			
		case APU_DMC_1_CPU_ADDR:
			mDmcReg.reg1 = value;
			break;
			
		case APU_DMC_2_CPU_ADDR:
			mDmcReg.reg2 = value;
			break;
			
		case APU_DMC_3_CPU_ADDR:
			mDmcReg.reg3 = value;
			break;
		
		case APU_STATUS_CPU_ADDR:
			mStatus = value;
			if ((mStatus & 0b0000'1000) == 0)
				mNoiseChannel.disable();
			else
				mNoiseChannel.enable();

			break;

		case APU_FRAME_COUNTER_CPU_ADDR:
			mFrameCounterReg = value;
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
		value = mStatus;
	else 
	{
		std::stringstream errorStream;
		errorStream << std::hex << "APU register 0x" << address << " is not readable.";
		testAndExitWithMessage(true, errorStream.str());
	}
	
	return value;
}

float APU::mix(u8 pulse1, u8 pulse2, u8 triangle, u8 noise, u8 dmc)
{
	float pulseOut, tndOut, tndTemp;
	u8 pulse12 = pulse1 + pulse2;
	u8 tnd = triangle + noise + dmc;

	// Pulses mix
	if (pulse12 == 0)
		pulseOut = 0.0f;
	else
		pulseOut = (95.88f * pulse12) / (8128 + 100.0f * pulse12);
	
	// Triangle, noise & DMC mix
	tndTemp = triangle * (1.0f / 8227) + 
	                noise * (1.0f / 12241) + 
					dmc * (1.0f / 22638);
	if (tnd == 0)
		tndOut = 0.0f;
	else
		tndOut = (159.79f * tndTemp) / (1.0f + 100.0f * tndTemp);
	
	return pulseOut + tndOut;
}
