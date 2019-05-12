#include "EepromWriterTask.h"

#include "ExpressTask.h"
#include "StaticAllocActivator.h"

#include "Globals.h"
#include "EepromUnload.h"
#include <EEPROM.h>
#include "OperatingMode.h"
#include "SoundTask.h"
#include "LcdInfoTask.h"
#include "Critical.h"

TASK_BEGIN(EepromWriterTask, {
	Record* r;
	unsigned offset;
	MyWriteQueue *queue;
})

queue = &Instance<MyWriteQueue>();

for (;;)
{
	if (queue->HasData())
	{
		if (!CheckMemoryFull())
		{
			digitalWrite(Pins::LED_WARNING, 1);
			r = &queue->Peek();

			for (offset = 0; offset < sizeof(Record); ++offset)
			{
				EEPROM[WriteOffset + offset] = ((byte*)r)[offset];
				TASK_YIELD();
			}
			WriteOffset += sizeof(Record);

			queue->Free(*r);
		}
		else
		{
			LcdModeNotifyEvent();
			Critical _;
			queue->Reset();
		}
	}
	else
	{
		digitalWrite(Pins::LED_WARNING, 0);
		TASK_YIELD();
	}

	if (CheckMemoryFull())
	{
		digitalWrite(Pins::LED_WARNING, 1);
		digitalWrite(Pins::LED_RISING, 1);
		digitalWrite(Pins::LED_FALLING, 1);
	}
}

TASK_END

bool RegisterEepromWriterTask(Scheduler& scheduler)
{
	return scheduler.Register(Instance<EepromWriterTask>());
}
