#include "LcdBacklightTask.h"
#include "ExpressTask.h"
#include "StaticAllocActivator.h"
#include "Globals.h"
#include "Median.h"

static Median<int, 7, byte> median;

TASK_BEGIN(LcdBacklightTask, {
	int value;
	int prev;
})

prev = -1;

TASK_PERIODICALLY(100, {
	value = analogRead(Pins::LCD_TUNE_BRIGHTNESS);

	if (value < 170)
	{
		value = 0;
	}
	else if (value < 170 + 255 * 8 / 3)
	{
		value -= 170;
		value *= 3;
		value >>= 3;
	}
	else
	{
		value = 255;
	}

	median.write(value);
	value = median.get();
	
	if (digitalRead(Pins::ENABLE_LIGHT) == 0)
	{
		analogWrite(Pins::LCD_SET_BRIGHTNESS, 0);
	}
	else
	{
		if (value != prev)
		{
			prev = value;
			analogWrite(Pins::LCD_SET_BRIGHTNESS, value);
		}
	}
});

TASK_END;

bool RegisterBacklightTask(Scheduler & scheduler)
{
	return scheduler.Register(Instance<LcdBacklightTask>());
}
