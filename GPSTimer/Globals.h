#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "Arduino.h"
#include "WriteQueue.h"
#include "DataRecord.h"


extern HardwareSerial& NMEA; // входной порт NMEA

extern HardwareSerial& ECHO; // порт получения информации. Для Arduino Mega или Micro может отличаться от NMEA.

extern unsigned WriteOffset;

extern unsigned WriteCapacity;

extern DateTime LastDateTime;
extern unsigned long LastDateTime_ms;
extern byte LastDateTime_version;

extern volatile unsigned long PpsTime;
extern volatile double TimeScale;
extern unsigned long ValidPpsTime;
extern DateTime ValidPpsDateTime;
extern byte ValidPpsTime_version;


extern Position LastPosition;
extern byte LastPosition_version;

extern volatile unsigned long SignalInterruptTime;

extern byte PressedButtons;

extern Record LastTakenRecord;

using MyWriteQueue = WriteQueue<Record, byte, 32>;

enum
{
	PPS_INTERRUPT_EVENT = RISING
};

extern byte writeQueue_version;

namespace Pins
{
	enum
	{
		MODE_SELECTOR = 11,

		LED_WARNING = LED_BUILTIN,

		LCD_BUTTONS = A0,

		LED_PPS = A1, // индикация получения PPS

		LED_RISING = A2, // индикция фронта сигнального прерывания

		LED_FALLING = A3, //индикция среза сигнального прерывания

		SOUND_TONE = 12,

		ENABLE_LIGHT = A4,

		LCD_TUNE_BRIGHTNESS = A5,

		LCD_SET_BRIGHTNESS = 10,

		INTERRUPT_PPS = 2,

		INTERRUPT_SIGNAL = 3,
	};
}

void UpdateVersion(byte& version) __attribute__((noinline));

#endif

