#ifndef _NMEAREADERTASK_h
#define _NMEAREADERTASK_h

#include "Arduino.h"
#include "Scheduler.h"

bool RegisterNmeaReaderTask(Scheduler& scheduler);

unsigned short GpsGetSatellitesNumber();

unsigned long GpsGetTotalChars();

unsigned short GpsGetGoodSentences();

unsigned short GpsGetFailedCS();

#endif

