#include "LedIndicatorTask.h"
#include "Arduino.h"
#include "ExpressTask.h"
#include "Critical.h"
#include "Globals.h"
#include "StaticAllocActivator.h"

volatile byte indicatorMask;

TASK_BEGIN(LedIndicatorTask, {
	byte maskCopy;
	unsigned long offPps;
	unsigned long offRising;
	unsigned long offFalling;
	unsigned long now;
})

pinMode(A4, INPUT);
digitalWrite(A4, 1);

pinMode(Pins::LED_PPS, OUTPUT);
pinMode(Pins::LED_RISING, OUTPUT);
pinMode(Pins::LED_FALLING, OUTPUT);

offPps = 0;
offRising = 0;
offFalling = 0;

TASK_YIELD();

while (true)
{
	{
		Critical __;
		maskCopy = indicatorMask;
		indicatorMask = 0;
	}
	
	now = millis();
	

	if (digitalRead(Pins::ENABLE_LIGHT))
	{
		if (maskCopy)
		{		
			unsigned long off = now + 200;

			if (bitRead(maskCopy, (byte)LedIndicator::PPS))
			{
				digitalWrite(Pins::LED_PPS, 1);
				offPps = off;
			}

			if (bitRead(maskCopy, (byte)LedIndicator::Rise))
			{
				digitalWrite(Pins::LED_RISING, 1);
				offRising = off;
			}

			if (bitRead(maskCopy, (byte)LedIndicator::Fall))
			{
				digitalWrite(Pins::LED_FALLING, 1);
				offFalling = off;
			}
		}
	}
	else
	{
		digitalWrite(Pins::LED_PPS, 0);
		digitalWrite(Pins::LED_RISING, 0);
		digitalWrite(Pins::LED_FALLING, 0);
	}
	

	if (offPps && (now >= offPps))
	{
		offPps = 0;
		digitalWrite(Pins::LED_PPS, 0);
	}

	if (offRising && (now >= offRising))
	{
		offRising = 0;
		digitalWrite(Pins::LED_RISING, 0);
	}

	if (offFalling && (now >= offFalling))
	{
		offFalling = 0;
		digitalWrite(Pins::LED_FALLING, 0);
	}

	TASK_YIELD();
}

TASK_END;

bool RegisterLedIndicator(Scheduler& scheduler)
{
	return scheduler.Register(Instance<LedIndicatorTask>());
}
