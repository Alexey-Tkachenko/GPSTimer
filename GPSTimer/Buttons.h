#ifndef _BUTTONS_H_
#define _BUTTONS_H_
#include "Arduino.h"


namespace Buttons
{
	const byte None = 0;

	const byte Select = 1;

	const byte Left = 2;

	const byte Up = 4;

	const byte Down = 8;

	const byte Right = 16;

	inline bool IsPressed(byte value, byte button)
	{
		return value & (byte)button;
	}

	inline void Reset(byte& value)
	{
		value = 0;
	}

	inline void Clear(byte& value, byte buttons)
	{
		value &= ~(byte)buttons;
	}

 	inline void Set(byte& value, byte buttons)
	{
		value |= (byte)buttons;
	}

	inline byte ReadRelease(byte& value)
	{
		byte tmp = value;
		value = 0;
		return tmp;
	}

	inline byte ReadRelease(byte& value, byte mask)
	{
		byte tmp = value & mask;
		value &= (byte)~mask;
		return tmp;
	}
}

#endif
