#pragma once

#include "NES/Config.hpp"
#include "NES/Divider.hpp"

class APUSweep
{
public:
	void reset();

	bool update(u16 timerPeriod, u16 targetPeriod);

	u16 calculateTargetPeriod1(u16 timerPeriod);
	u16 calculateTargetPeriod2(u16 timerPeriod);

	inline bool isMuting(u16 timerPeriod, u16 targetPeriod)
	{
		return (timerPeriod < 8 || targetPeriod > 0x7FF);
	}

	inline void setEnableFlag(bool isEnabled) { mIsEnabled = isEnabled; }
	inline void setDividerPeriod(u16 period) { mDivider.loadPeriod(period); }
	inline void setNegateFlag(bool isNegating) { mIsNegating = isNegating; }
	inline void setShiftCount(u8 shiftCount) { mShiftCount = shiftCount; }

	inline void setReloadFlag() { mIsReloadFlagSet = true; }
	
private:
	Divider mDivider;
	u8 mShiftCount;
	bool mIsEnabled;
	bool mIsNegating;
	bool mIsReloadFlagSet;
};
