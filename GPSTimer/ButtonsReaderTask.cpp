#include "ButtonsReaderTask.h"

#include "Arduino.h"
#include "ExpressTask.h"
#include "StaticAllocActivator.h"
#include "Globals.h"
#include "Buttons.h"
#include "SoundTask.h"

static const int FirstTick = 1000;
static const int NextTick = 600;

TASK_BEGIN(ButtonsReaderTask, { 
	int adc; 
	byte prevButtons;
	byte current;
	unsigned long nextTick;
})

Buttons::Reset(::PressedButtons);

for (;;)
{
	adc = analogRead(Pins::LCD_BUTTONS);
	/*
	None - 1023

	Right - 0
	Up - 131
	Down - 306
	Left - 479
	Select - 720
	*/
	if (adc < 60)
	{
		current = Buttons::Right;
	}
	else if (adc < 220)
	{
		current = Buttons::Up;
	}
	else if (adc < 393)
	{
		current = Buttons::Down;
	}
	else if (adc < 600)
	{
		current = Buttons::Left;
	}
	else if (adc < 872)
	{
		current = Buttons::Select;
	}
	else
	{
		current = Buttons::None;
	}

	if (current != Buttons::None)
	{
		if (prevButtons != current)
		{
			Buttons::Set(::PressedButtons, current);
			nextTick = millis() + FirstTick;
			PlaySound(SoundType::ButtonClick);
		}
		else
		{
			if (millis() > nextTick)
			{
				Buttons::Set(::PressedButtons, current);
				nextTick += NextTick;
				PlaySound(SoundType::ButtonClick);
			}
		}
	}
	prevButtons = current;

	TASK_SLEEP(10);
}

TASK_END;

bool RegisterButtonsReaderTask(Scheduler& scheduler)
{
	return scheduler.Register(Instance<ButtonsReaderTask>());
}
