#include "Globals.h"

#include "Arduino.h"

HardwareSerial& NMEA = Serial;

HardwareSerial& ECHO = Serial;

unsigned WriteOffset;

unsigned WriteCapacity;

DateTime LastDateTime;
unsigned long LastDateTime_ms;
byte LastDateTime_version;

volatile unsigned long PpsTime;
volatile double TimeScale = 1;
unsigned long ValidPpsTime;
DateTime ValidPpsDateTime;
byte ValidPpsTime_version;

Position LastPosition;
byte LastPosition_version;

volatile unsigned long SignalInterruptTime;

byte writeQueue_version;

Record LastTakenRecord;

byte PressedButtons;

void UpdateVersion(byte& version)
{
	if (++version == 0)
	{
		version = 1;
	}
}
