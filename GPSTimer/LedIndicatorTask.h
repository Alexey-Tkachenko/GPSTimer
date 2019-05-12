// LedIndicator.h

#ifndef _LEDINDICATOR_h
#define _LEDINDICATOR_h
#include "Scheduler.h"

enum class LedIndicator : byte
{
	PPS,
	Rise,
	Fall
};

bool RegisterLedIndicator(Scheduler& scheduler);

template<LedIndicator indicator>
inline void ShowLedIndicator()
{
	extern volatile byte indicatorMask;
	_SFR_BYTE(indicatorMask) |= (1 << (byte)indicator);
}

#endif

