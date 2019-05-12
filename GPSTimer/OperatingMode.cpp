#include "OperatingMode.h"
#include "Arduino.h"
#include "Scheduler.h"
#include "StaticAllocActivator.h"
#include "LcdIo.h"

#include "Globals.h"

#include "SoundTask.h"
#include "LedIndicatorTask.h"
#include "LcdBacklightTask.h"
#include "ButtonsReaderTask.h"
#include "LcdInfoTask.h"

#include "EepromWriterTask.h"
#include "NmeaReaderTask.h"

#include "Assert.h"
#include "EepromUnload.h"

#include "ExpressTask.h"

#include "Optimize.h"

#include <EEPROM.h>

static Scheduler scheduler;

bool ScanMemory()
{
	lcdPrint(0, F("Scanning data"), false);
	delay(200);
	int write = 0;
	int read = 0;
	int end = EEPROM.length() - sizeof(Record) + 1;
	bool first = true;

	while (read < end)
	{
		Record r;
		// ������ ������
		EEPROM.get(read, r);
		
		if (r.Check == Record::OK) 
		{
			if (read != write)
			{
				if (first)
				{
					first = false;
					lcdPrint(0, F("Compressing data"), false);
					delay(200);
				}
				EEPROM.put(WriteOffset, r);
				for (byte i = 0; i < sizeof(Record); ++i)
				{
					EEPROM.write(read + i, 0);
				}
			}
			write += sizeof(Record);
		}
		read += sizeof(Record);
	}

	::WriteOffset = write;
	::WriteCapacity = EEPROM.length() / sizeof(Record);

	lcdClearLine(1);
	DisplayStatistics();

	delay(2000);
	lcdClear();
	return !CheckMemoryFull();
}

void DisplayStatistics()
{
	int freeData = (EEPROM.length() - WriteOffset) / sizeof(Record);

	if (freeData)
	{
		static char buf[16 + 1];
		strcpy_P(buf, PSTR("Free "));
		itoa(freeData, buf + strlen(buf), 10);
		strcat_P(buf, PSTR(" of "));
		itoa(::WriteCapacity, buf + strlen(buf), 10);
		lcdPrint(0, buf, false);
	}
	else
	{
		lcdPrint(0, F("Memory is full"), false);
	}
}

volatile bool ignoreSignal;
static void onSignal() OPT_FAST;

void onPPS() OPT_FAST;

static void SetupOperating()
{
	ScanMemory();

	ASSERT_A(RegisterNmeaReaderTask(scheduler));
	
	ASSERT_A(RegisterEepromWriterTask(scheduler));

	ASSERT_A(RegisterButtonsReaderTask(scheduler));

	ASSERT_A(RegisterLcdInfoTask(scheduler));
	
	ASSERT_A(RegisterBacklightTask(scheduler));

	ASSERT_A(RegisterLedIndicator(scheduler));

	ASSERT_A(RegisterSoundPlayer(scheduler));

	attachInterrupt(digitalPinToInterrupt(Pins::INTERRUPT_PPS), onPPS, PPS_INTERRUPT_EVENT);

	pinMode(Pins::INTERRUPT_SIGNAL, INPUT);
	digitalWrite(Pins::INTERRUPT_SIGNAL, 1);
	ignoreSignal = true;
	attachInterrupt(digitalPinToInterrupt(Pins::INTERRUPT_SIGNAL), onSignal, CHANGE);
	delay(10);
	ignoreSignal = false;
}

void PerformOperating()
{
	lcdPrint(F("Mode: Operating"));
	delay(1000);
	
	SetupOperating();
	
	for (;;)
	{
		scheduler.Invoke();
	}
}


static unsigned long signalWaitInterval;

static void __attribute__((noinline)) onSignal2(unsigned long uNow)
{
    if (uNow < signalWaitInterval)
    {
        return;
    }
    signalWaitInterval = uNow + 5000L;

    if (ValidPpsTime_version == 0)
    {
        PlaySound(SoundType::SignalFail);
        return;
    }

    // ��������� ������� ��������� ������� �����.
    // ���� ��� ����� 1, �� �������� ������� �� ������� � ������� ���������, ���� 0 - �� �� �������� � ������.
    byte v = digitalRead(Pins::INTERRUPT_SIGNAL);

    // ��������� ������� ������������ �������
    Record& r = Instance<MyWriteQueue>().Allocate();

    // ����� ������������ ����
    r.Check = 0;
    // ������ �������������� �����
    r.Time = ValidPpsDateTime;
    r.Position = LastPosition;
    r.Type = v;
    r.Offset = (uNow - ValidPpsTime) * TimeScale;
    // ��������� ������������ ����
    r.Check = Record::OK;

    Instance<MyWriteQueue>().Complete(r);
    LastTakenRecord = r;
    UpdateVersion(writeQueue_version);


    // �����������
    if (v)
    {
        ShowLedIndicator<LedIndicator::Rise>();
        PlaySound(SoundType::SignalRise);
    }
    else
    {
        ShowLedIndicator<LedIndicator::Fall>();
        PlaySound(SoundType::SignalFall);
    }
    LcdModeNotifyEvent();
}

static void onSignal() 
{
    unsigned long now = micros();
    if (ignoreSignal) return;
    // 2-������ ����� ��� ����, ����� � ������� ������ micros() �� ��������� ����� �� ���������� ��������� � �����
    onSignal2(now);
}

