#ifndef _SOUND_H_
#define _SOUND_H_

#include "Scheduler.h"

void DisableSound(bool disable);
bool DisableSound();

enum class SoundType : byte
{
    None,
    SignalFail, // (400,300)
    SignalRise, // (800,100);
    SignalFall, // (600,100)

    ButtonClick, //(300,50)

    PPS, //(1000, 150)
};


void PlaySound(SoundType type);

bool RegisterSoundPlayer(Scheduler& scheduler);

#endif
