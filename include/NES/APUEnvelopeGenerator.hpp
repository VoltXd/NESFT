#pragma once

#include "NES/Config.hpp"
#include "NES/APUDivider.hpp"

class APUEnvelopeGenerator
	{
	public:
		void reset(); 

		void onClock();
		inline u8 getOutput() { return mIsConstantVolume ? mVolume : mDecayCounter; }

		void setVolume(u8 volume);
		inline void setLoopFlag(bool isLoopFlagSet) { mIsLoopFlagSet = isLoopFlagSet; }
		inline void setConstantVolumeFlag(bool isConstantVolume) { mIsConstantVolume = isConstantVolume; }

		inline void setStartFlag() { mIsStartFlagClear = false; }

	private:
		static constexpr u8 DECAY_COUNTER_PERIOD = 15;

		APUDivider mDivider;
		u8 mDecayCounter;
		u8 mVolume;
		bool mIsStartFlagClear;
		bool mIsLoopFlagSet;
		bool mIsConstantVolume;
	};