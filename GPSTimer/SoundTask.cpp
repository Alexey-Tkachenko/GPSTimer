#include "SoundTask.h"
#include "ExpressTask.h"
#include "StaticAllocActivator.h"
#include "Critical.h"
#include "Globals.h"
#include "GlobalConfig.h"

void DisableSound(bool disable)
{
	ConfigLifeTime _;
	Config().DisableSound = disable;
}

bool DisableSound()
{
	return Config().DisableSound;
}

static WaitHandles::ValueHolder<SoundType> latch;

TASK_BEGIN(SoundPlayer, { SoundType r; })

for (;;)
{
	TASK_WAIT_FOR(&latch);
	{
		Critical _;
		r = latch.Get();
	}
	if (!Config().DisableSound)
	{
        unsigned frequency;
        unsigned duration;

        switch (r)
        {
        case SoundType::None:
            continue;
        case SoundType::SignalFail:
            frequency = 400;
            duration = 300;
            break;
        case SoundType::SignalRise:
            frequency = 800;
            duration = 100;
            break;
        case SoundType::SignalFall:
            frequency = 600;
            duration = 100;
            break;
        case SoundType::ButtonClick:
            frequency = 300;
            duration = 50;
            break;
        case SoundType::PPS:
            frequency = 1000;
            duration = 150;
            break;
        default:
            continue;
        }

		tone(Pins::SOUND_TONE, frequency, duration);
	}
}

TASK_END;

void PlaySound(SoundType type)
{
    latch.Set(type);
}

bool RegisterSoundPlayer(Scheduler & scheduler)
{
	return scheduler.Register(Instance<SoundPlayer>());
}
