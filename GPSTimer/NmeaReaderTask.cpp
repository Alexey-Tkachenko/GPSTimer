#include "NmeaReaderTask.h"
#include "Arduino.h"

#include "ExpressTask.h"
#include "StaticAllocActivator.h"
#include "Globals.h"
#include "DataRecord.h"
#include "Critical.h"
#include "TinyGPS-mod.h"

#include "LcdIo.h"

#include "SoundTask.h"
#include "LedIndicatorTask.h"


static TinyGPS_mod gps;
static unsigned short statSatelliteNumber;
static unsigned long statChars;
static unsigned short statGoodSentences;
static unsigned short statFailedCS;


TASK_BEGIN(NmeaReaderTask, 
{ 
	bool newData;
	int c;
})

digitalWrite(Pins::LED_PPS, 1);

for (;;)
{
	TASK_YIELD_WHILE(!NMEA.available());
	while (NMEA.available())
	{
		c = NMEA.read();
		ECHO.write(c);
		if (gps.encode(c))
		{
			newData = true;
		}
	}
	
	if (newData)
	{
		newData = false;
        unsigned long ppsTime;
        {
            Critical _;
            ppsTime = ::PpsTime;
        }

        if (!ppsTime) // no PPS fixed
        {
            continue;
        }

        if (micros() - ppsTime > 900000) // data too old (900 ms)
        {
            continue;
        }

		int year;
		byte month, day, hour, minute, second, hundredths;
		unsigned long age;
		
		gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
		year -= 2000;
		statSatelliteNumber = gps.satellites();
		gps.stats(&statChars, &statGoodSentences, &statFailedCS);
		
		float lon, lat;
		gps.f_get_position(&lat, &lon, &age);
		int alt = (gps.altitude() + 50) / 100;

        Critical _;
        
        LastPosition = { lat, lon, alt };
        UpdateVersion(LastPosition_version);
        
        DateTime now = { (byte)year, month, day, hour, minute, second };
        if (LastDateTime.t.second != second && (micros() - ppsTime < 900000))
		{
			ValidPpsTime = ppsTime;
            ValidPpsDateTime = now;
            UpdateVersion(ValidPpsTime_version);
		}
        LastDateTime = now;
        UpdateVersion(LastDateTime_version);
        LastDateTime_ms = millis();
	}
}

TASK_END;

static void __attribute__((noinline)) onPPS2(unsigned long now)
{

    unsigned long delta = now - PpsTime;
    if ((delta > 900000) && (delta < 1100000))
    {
        TimeScale = 1.0e6 / delta;
    }

    PpsTime = now;
    ShowLedIndicator<LedIndicator::PPS>();
    PlaySound(SoundType::PPS);
}

void onPPS()
{
    onPPS2(micros());
}


bool RegisterNmeaReaderTask(Scheduler& scheduler)
{
	return scheduler.Register(Instance<NmeaReaderTask>(), TaskPriority::SensorPoll);
}


unsigned short GpsGetSatellitesNumber()
{
	return statSatelliteNumber;
}

unsigned long GpsGetTotalChars()
{
	return statChars;
}

unsigned short GpsGetGoodSentences()
{
	return statGoodSentences;
}

unsigned short GpsGetFailedCS()
{
	return statFailedCS;
}
