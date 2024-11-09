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
			break;
			
		case APU_NOISE_1_CPU_ADDR:
			// Unused register
			break;
			
		case APU_NOISE_2_CPU_ADDR:
			mNoiseReg.reg1 = value;
			break;
			
		case APU_NOISE_3_CPU_ADDR:
			mNoiseReg.reg2 = value;
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
			break;

		case APU_FRAME_COUNTER_CPU_ADDR:
			mFrameCounter = value;
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
