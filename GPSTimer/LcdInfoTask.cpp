#include "LcdInfoTask.h"
#include "Arduino.h"
#include "StaticAllocActivator.h"
#include "ExpressTask.h"
#include "LcdIo.h"
#include "Globals.h"
#include "OperatingMode.h"
#include "Buttons.h"
#include "Assert.h"
#include "SoundTask.h"

#include "NmeaReaderTask.h"
#include "EepromUnload.h"
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include "DataRecord.h"

enum class LcdMode : byte
{
	Unknown,
	FIRST = 1,

	EventData = 1,
	MemoryStatistics,
	DateTime,
	GpsLocation,
	GpsStatistics,
	SoundSettings,

	OUT_OF_RANGE,
};

static volatile LcdMode _newMode;
static int _dataIndex = -1;

void LcdModeNotifyEvent()
{
	_newMode = LcdMode::EventData;
	_dataIndex = -1;
}

static inline const __FlashStringHelper* noGpsData()
{
	return F("No GPS data");
}

//void SetLcdMode(LcdMode mode)
//{
//	newMode = mode;
//}

TASK_BEGIN(LcdInfoTask, {
	LcdMode mode;
	bool modeChanged;
	byte prevPositionVersion;
	byte prevDateTimeVersion;
	byte prevWriteQueueVersion;
	
	byte buttonScan;

	char buffer[17];

	Position pos;
	DateTime time;
	
	signed char statisticsIndex;
	signed char locationIndex;

	bool UseCustomSelectKey;
	bool SelectPressed;

	byte tmpIndex;

	int dataModeIndex;
	Record dataCopy;
})


for (;;)
{
	if (writeQueue_version != prevWriteQueueVersion)
	{
		prevWriteQueueVersion = writeQueue_version;
		LcdModeNotifyEvent();
	}

	SelectPressed = false;

	buttonScan |= Buttons::ReadRelease(::PressedButtons);

	if (Buttons::IsPressed(buttonScan, Buttons::Select))
	{
		DBG_SEL(__LINE__);
		if (UseCustomSelectKey)
		{
			DBG_SEL(__LINE__);
			SelectPressed = true;
			DBG_SEL(__LINE__);
		}
		else
		{
			DBG_SEL(__LINE__);
			_newMode = NextMode(mode);
		}
		Buttons::Reset(buttonScan);
	}
	DBG_SEL(__LINE__);
	UseCustomSelectKey = false;
	DBG_SEL(__LINE__);

	if (mode == LcdMode::Unknown)
	{
		_newMode = mode = NextMode(LcdMode::Unknown);
		modeChanged = true;
	}
	else
	{
		modeChanged = false;
		if (_newMode != mode)
		{
			switch (mode)
			{
			case LcdMode::EventData:
				ClearDisplayEventData();
				break;
			case LcdMode::GpsLocation:
				ClearDisplayGpsLocation();
				break;
			case LcdMode::DateTime:
				ClearDisplayDateTime();
				break;
			case LcdMode::MemoryStatistics:
				ClearMemoryStatistics();
				break;
			case LcdMode::GpsStatistics:
				ClearGpsStatistics();
				break;
			case LcdMode::SoundSettings:
				ClearSoundSettings();
				break;
			default:
				break;
			}
			mode = _newMode;
			modeChanged = true;
			lcdClear();
		}
	}

	switch (mode)
	{
	case LcdMode::EventData:
		DisplayEventData();
		break;
	case LcdMode::GpsLocation:
		DisplayGpsLocation();
		break;
	case LcdMode::DateTime:
		DisplayDateTime();
		break;
	case LcdMode::MemoryStatistics:
		DisplayMemoryStatistics();
		break;
	case LcdMode::GpsStatistics:
		DisplayGpsStatistics();
		break;
	case LcdMode::SoundSettings:
		DisplaySoundSettings();
		break;
	default:
		break;
	}

	TASK_SLEEP(100);
}

TASK_BODY_END;

static LcdMode NextMode(LcdMode prev)
{
	LcdMode newMode = (LcdMode)((byte)prev + 1);
	if (newMode == LcdMode::OUT_OF_RANGE)
	{
		newMode = LcdMode::FIRST;
	}
	return newMode;
}

void ClearDisplayGpsLocation()
{
	prevPositionVersion = 0;
	locationIndex = 0;
}

void DisplayGpsLocation()
{
	if (LastPosition_version)
	{
		if (prevPositionVersion == 0)
		{
			lcdClear();
		}

		bool left = Buttons::IsPressed(buttonScan, Buttons::Left);
		bool right = Buttons::IsPressed(buttonScan, Buttons::Right);
		Buttons::Clear(buttonScan, Buttons::Left | Buttons::Right);

		bool updateRequired = false;

		if (left != right)
		{
			if (left)
			{
				if (--locationIndex < 0)
				{
					locationIndex += 3;
				}
			}
			else
			{
				if (++locationIndex >= 3)
				{
					locationIndex -= 3;
				}
			}
			updateRequired = true;
			lcdClear();
		}
	
		if (modeChanged || prevPositionVersion != LastPosition_version)
		{
			pos = LastPosition;
			updateRequired = true;
		}

		if (updateRequired)
		{
			switch (locationIndex)
			{
			case 0:
				ShowAngle(pos);
				break;
			case 1:
                ShowAngle2(pos);
                break;
			case 2:
                ShowAltitude(pos.Altitude);
				break;
			default:
                locationIndex = 0;
                break;
			}
		}
	}
	else
	{
		if (modeChanged)
		{
			lcdPrint(noGpsData(), F("Pos. unavailable"));
		}
		Buttons::Clear(buttonScan, Buttons::Left | Buttons::Right);
	}
	prevPositionVersion = LastPosition_version;
}


void ClearDisplayDateTime()
{
	prevDateTimeVersion = 0;
}

static char* FormatDate(char *buffer, byte size, const DateTime& t, bool shortYear = false)
{
	byte year = t.t.year;
	byte month = t.t.month;
	byte day = t.t.day;

	put(buffer, size, '0' + (day / 10));
	put(buffer, size, '0' + (day % 10));
	put(buffer, size, '-');
	put(buffer, size, '0' + (month > 9));
	put(buffer, size, '0' + (month > 9 ? month - 10 : month));
	put(buffer, size, '-');
	if (shortYear == false)
	{
		put(buffer, size, '2');
		put(buffer, size, '0');
	}
	put(buffer, size, '0' + (year / 10));
	put(buffer, size, '0' + (year % 10));

	return buffer;
}

static char* FormatTime(char* buffer, byte size, const DateTime& t, bool gmtSuffix = false)
{
	byte hour = t.t.hour;
	byte minute = t.t.minute;
	byte second = t.t.second;

	put(buffer, size, '0' + (hour / 10));
	put(buffer, size, '0' + (hour % 10));
	put(buffer, size, ':');
	put(buffer, size, '0' + (minute / 10));
	put(buffer, size, '0' + (minute % 10));
	put(buffer, size, ':');
	put(buffer, size, '0' + (second / 10));
	put(buffer, size, '0' + (second % 10));

	if (gmtSuffix)
	{
		put(buffer, size, ' ');
		put(buffer, size, 'U');
		put(buffer, size, 'T');
	}

	int delta = (millis() - LastDateTime_ms) / 1000;
	if (delta > 0) 
	{
		if (delta > 1000) 
		{
            put(buffer, size, ' ');
            put(buffer, size, 'O');
			put(buffer, size, 'L');
			put(buffer, size, 'D');
		}
		else
		{
			put(buffer, size, '+');

			if (delta >= 100) 
			{
				put(buffer, size, delta / 100 + '0');
				delta %= 100;
				put(buffer, size, delta / 10 + '0');
				delta %= 100;
				put(buffer, size, delta + '0');
			}
			else if (delta >= 10) 
			{
				put(buffer, size, delta / 10 + '0');
				delta %= 10;
				put(buffer, size, delta + '0');
			}
			else
			{
				put(buffer, size, delta + '0');
			}
            put(buffer, size, 's');
		}
	}

	return buffer;
}

void DisplayDateTime()
{
	if (LastDateTime_version)
	{
		if (prevDateTimeVersion == 0)
		{
			lcdClear();
		}
		if (prevDateTimeVersion != LastDateTime_version)
		{
			time = LastDateTime;
			*FormatDate(buffer, 16, time) = 0;
			lcdPrint(buffer, 0, false);
			*FormatTime(buffer, 16, time, true) = 0;
			lcdPrint(0, buffer, false);
		}
	}
	else
	{
		if (modeChanged)
		{
			lcdPrint(noGpsData(), F("Time unavailable"));
		}
	}
}


void ClearMemoryStatistics() {}

signed char memoryIndex;

void DisplayMemoryStatistics()
{
	bool left = Buttons::IsPressed(buttonScan, Buttons::Left);
	bool right = Buttons::IsPressed(buttonScan, Buttons::Right);
	Buttons::Clear(buttonScan, Buttons::Left | Buttons::Right);

	bool updateRequired = modeChanged;
	

	if (left != right)
	{
		if (left)
		{
			if (--memoryIndex < 0)
			{
				memoryIndex += 4;
			}
		}
		else
		{
			if (++memoryIndex >= 4)
			{
				memoryIndex -= 4;
			}
		}
		lcdClear();
		updateRequired = true;
	}

	if (modeChanged)
	{
		memoryIndex = 0;
	}

	updateRequired |= SelectPressed;
	
	switch (memoryIndex)
	{
	case 0:
		if (updateRequired)
		{
			lcdPrint(F("Memory usage"), 0, false);
			::DisplayStatistics();
		}
		break;
	case 1:
		DBG_SEL(__LINE__);
		UseCustomSelectKey = true;
		DBG_SEL(__LINE__);
		if (SelectPressed)
		{
			::ScanMemory();
		}
		if (updateRequired)
		{
			lcdPrint(F("Pack memory"), F("[SEL] - pack"));
		}
		break;
	case 2:
		DBG_SEL(__LINE__);
		UseCustomSelectKey = true;
		DBG_SEL(__LINE__);
		if (SelectPressed)
		{
            lcdPrint(F("Erasing..."));
            EraseData();
            WriteOffset = 0;
            memoryIndex = 0;
            updateRequired = true;
		}
		if (updateRequired)
		{
			lcdPrint(F("Erase memory"), F("[SEL] - erase"));
		}
		break;
	case 3:
		DBG_SEL(__LINE__);
		UseCustomSelectKey = true;
		DBG_SEL(__LINE__);
		if (SelectPressed)
		{
			DownloadData();
			memoryIndex = 0;
		}
		else
		{
			if (updateRequired)
			{
				lcdPrint(F("Download memory"), F("[SEL] - download"));
			}
		}
		break;
	}
}

void ClearGpsStatistics() {}

void DisplayGpsStatistics()
{
	bool left = Buttons::IsPressed(buttonScan, Buttons::Left);
	bool right = Buttons::IsPressed(buttonScan, Buttons::Right);
	Buttons::Clear(buttonScan, Buttons::Left | Buttons::Right);

	if (left != right)
	{
		if (left)
		{
			if (--statisticsIndex < 0)
			{
				statisticsIndex += 4;
			}
		}
		else
		{
			if (++statisticsIndex >= 4)
			{
				statisticsIndex -= 4;
			}
		}
		lcdClear();
	}

	if (modeChanged)
	{
		lcdClear();
	}

	switch (statisticsIndex)
	{
	case 0:
		lcdPrint(F("GNSS satellites"), 0, false);
		{
			auto tmp = ::GpsGetSatellitesNumber();
			char* _buf = buffer;
			byte _size = 16;
			if (tmp >= 10)
			{
				put(_buf, _size, '0' + tmp / 10);
				put(_buf, _size, '0' + tmp % 10);
			}
			else
			{
				put(_buf, _size, '0' + tmp);
				put(_buf, _size, ' ');
			}
			put(_buf, _size, 0);
		}
		lcdPrint(0, buffer, false);
		break;
	case 1:
		lcdPrint(F("GNSS chars"), 0, false);
		ltoa(::GpsGetTotalChars(), buffer, 10);
		lcdPrint(0, buffer, false);
		break;
	case 2:
		lcdPrint(F("GNSS sentences"), 0, false);
		itoa(::GpsGetGoodSentences(), buffer, 10);
		lcdPrint(0, buffer, false);
		break;
	case 3:
		lcdPrint(F("GNSS failed CS"), 0, false);
		itoa(::GpsGetFailedCS(), buffer, 10);
		lcdPrint(0, buffer, false);
		break;
	default:
        statisticsIndex = 0;
		break;
	}
}

void ClearDisplayEventData()
{
	_dataIndex = -1;
}

void DisplayEventData()
{

	bool updateRequired = modeChanged;
	
	int last = ::WriteOffset / sizeof(Record) - 1;

	if (last == -1)
	{
		if (updateRequired)
		{
			lcdPrint(F("No records"));
		}
		return;
	}

	if (_dataIndex == -1)
	{
		_dataIndex = last;
		updateRequired = true;
		EEPROM.get(_dataIndex * sizeof(Record), dataCopy);
	}
	

	bool up = Buttons::IsPressed(buttonScan, Buttons::Up);
	bool down = Buttons::IsPressed(buttonScan, Buttons::Down);
	Buttons::Clear(buttonScan, Buttons::Up | Buttons::Down);

	if (up != down)
	{
		if (down)
		{
			if (--_dataIndex < 0)
			{
				_dataIndex = last;
			}
		}
		else
		{
			if (++_dataIndex > last)
			{
				_dataIndex = 0;
			}
		}
		EEPROM.get(_dataIndex * sizeof(Record), dataCopy);
		updateRequired = true;
		lcdClear();
	}

	bool left = Buttons::IsPressed(buttonScan, Buttons::Left);
	bool right = Buttons::IsPressed(buttonScan, Buttons::Right);
	Buttons::Clear(buttonScan, Buttons::Left | Buttons::Right);

	if (left != right)
	{
		if (left)
		{
			if (--dataModeIndex < 0)
			{
				dataModeIndex += 6;
			}
		}
		else
		{
			if (++dataModeIndex >= 6)
			{
				dataModeIndex -= 6;
			}
		}
		updateRequired = true;
		lcdClear();
	}

	switch (dataModeIndex) 
	{
	case 0:
		if (updateRequired)
		{
			lcdPrint(F("Rec "));
			lcdPutLineAt(0, 4, F("    "));
			itoa(_dataIndex + 1, buffer, 10);
			lcdPutLineAt(0, 4, buffer);
			*FormatTime(buffer, 16, dataCopy.Time) = 0;
			lcdPutLineAt(0, 8, buffer);
			lcdPutCharAt(1, 0, dataCopy.Type ? SYMBOL_RISE : SYMBOL_FALL);
			*FormatTimeOffset(buffer, 10, dataCopy.Offset) = 0;
			lcdPutLineAt(1, 2, buffer);

			switch (dataCopy.Check)
			{
			case Record::OK:
				lcdPutLineAt(1, 14, F("OK"));
				break;
			case Record::ERASED:
				lcdPutLineAt(1, 13, F("DEL"));
				break;
			default:
				itoa(dataCopy.Check, buffer, 16);
				lcdPutLineAt(1, 12, buffer);
				break;
			}
		}
		break;
	case 1:
		if (updateRequired)
		{
			char *tmp = FormatDate(buffer, 16, dataCopy.Time);
			*tmp++ = ' ';
			*tmp++ = '#';
			itoa(_dataIndex + 1, tmp, 10);
			lcdPrint(buffer);
			*FormatTime(buffer, 16, dataCopy.Time, true) = 0;
			lcdPrint(0, buffer, false);
		}
		break;
	case 2:
		if (updateRequired)
		{
			ShowAngle(dataCopy.Position);
		}
		break;
	case 3:
		if (updateRequired)
		{
			ShowAngle2(dataCopy.Position);
		}
		break;
	case 4:
		if (updateRequired)
		{
			ShowAltitude(dataCopy.Position.Altitude);
		}
		break;
	case 5:
		DBG_SEL(__LINE__);
		UseCustomSelectKey = true;
		DBG_SEL(__LINE__);
		if (SelectPressed)
		{
			DBG_SEL(__LINE__);
			switch (dataCopy.Check)
			{
			case Record::OK:
				dataCopy.Check = Record::ERASED;
				break;
			case Record::ERASED:
				dataCopy.Check = Record::OK;
				break;
			}
			
			EEPROM.put(_dataIndex * sizeof(Record) + offsetof(Record, Check), dataCopy.Check);
			updateRequired = true;
		}
		if (updateRequired)
		{
			switch (dataCopy.Check)
			{
			case Record::OK:
				lcdPrint(F("#     Erase?"), F("[SEL] to erase"));
				break;
			case Record::ERASED:
				lcdPrint(F("#     Restore?"), F("[SEL] to restore"));
				break;
			}
		}
		itoa(_dataIndex + 1, buffer, 10);
		lcdPutLineAt(0, 1, buffer);
		break;
	default:
        dataModeIndex = 0;
		break;
	}
}

void ClearSoundSettings() {}

void DisplaySoundSettings()
{
	bool left = Buttons::IsPressed(buttonScan, Buttons::Left);
	bool right = Buttons::IsPressed(buttonScan, Buttons::Right);
	Buttons::Clear(buttonScan, Buttons::Left | Buttons::Right);

	bool updateRequired = false;

	if (left != right)
	{
		DisableSound(!DisableSound());
		updateRequired = true;
	}

	if (modeChanged)
	{
		updateRequired = true;
	}

	if (updateRequired)
	{
		if (DisableSound())
		{
			lcdPrint(F("Sound: OFF"), F("LEFT/RIGHT - ON"));
		}
		else
		{
			lcdPrint(F("Sound: ON"), F("LEFT/RIGHT - OFF"));
		}
	}
}

static inline void put(char* &buffer, byte &size, char c)
{
	if (size)
	{
		*buffer = c;
		++buffer;
		--size;
	}
}

static char* FormatAngle2(char* buffer, byte size, float angle, const char* sign)
{
	bool negative = angle < 0;
	angle = fabs(angle) + 0.5e-5;
	
	int v = (int)angle;
	if (v > 100)
	{
		put(buffer, size, '1');
		v -= 100;
	}
	put(buffer, size, '0' + (v / 10));
	put(buffer, size, '0' + (v % 10));
	put(buffer, size, '.');

	angle -= v;
	for (byte i = 0; i < 5; ++i)
	{
		angle *= 10;
		v = (int)angle;
		angle -= v;
		put(buffer, size, '0' + v);
	}
	put(buffer, size, SYMBOL_DEG);
	put(buffer, size, sign[negative]);

	return buffer;
}

static char* FormatAngle(char* buffer, byte size, float angle, const char* sign)
{
	int v;
	bool negative = angle < 0;
	angle = fabs(angle);

	angle += 0.05 / 3600;
	v = int(angle);
	angle -= v;
	
	if (v >= 100)
	{
		put(buffer, size, '1');
		v -= 100;
	}
	put(buffer, size, '0' + (v / 10));
	put(buffer, size, '0' + (v % 10));

	put(buffer, size, SYMBOL_DEG);
	
	angle *= 60;
	v = (int)angle;
	angle -= v;

	put(buffer, size, '0' + (v / 10));
	put(buffer, size, '0' + (v % 10));

	put(buffer, size, SYMBOL_MIN);

	angle *= 60;
	v = (int)angle;
	angle -= v;
	put(buffer, size, '0' + (v / 10));
	put(buffer, size, '0' + (v % 10));
	put(buffer, size, '.');
	angle *= 10;
	v = (int)angle;
	put(buffer, size, '0' + (v % 10));
	put(buffer, size, SYMBOL_SEC);

	put(buffer, size, ' ');

	put(buffer, size, sign[negative]);

	return buffer;
}

static char* FormatTimeOffset(char* buffer, byte size, unsigned long offset)
{
	unsigned long sec = offset / 1000000ull;
	unsigned long usec = offset % 1000000ull;

	if (sec == 0)
	{
		put(buffer, size, '0');
	}
	else
	{
		char c[10];
		byte idx = 0;
		while (sec)
		{
			c[idx] = sec % 10;
			sec /= 10;
			++idx;
		}
		while (idx)
		{
			put(buffer, size, '0' + c[idx - 1]);
			--idx;
		}
	}
	put(buffer, size, '.');
	
	{
		char c[6];
		byte idx = 0;
		while (usec)
		{
			c[idx] = usec % 10;
			usec /= 10;
			++idx;
		}
		while (idx < 6)
		{
			c[idx++] = 0;
		}
		while (idx)
		{
			put(buffer, size, '0' + c[idx - 1]);
			--idx;
		}
	}
	return buffer;
}

static constexpr const char* ns = "NS";
static constexpr const char* ew = "EW";

void ShowAngleInternal(const Position& pos, char*(*fmt)(char*, byte, float, const char*))
{
	*fmt(buffer, 16, pos.Latitude, ns) = 0;
	lcdPrint(buffer, 0, false);
	*fmt(buffer, 16, pos.Longitude, ew) = 0;
	lcdPrint(0, buffer, false);
}

void ShowAngle(const Position& pos)
{
	ShowAngleInternal(pos, FormatAngle);
}

void ShowAngle2(const Position& pos)
{
	ShowAngleInternal(pos, FormatAngle2);
}

void ShowAltitude(int alt)
{
	lcdPrint(F("Alt"), 0, false);
	itoa(alt, buffer, 10);
	lcdPrint(0, buffer, false);
}

void DBG_SEL(int line)
{
	//Serial.print(F("Line: "));
	//Serial.print(line);
	//Serial.print(F("\t   Use: "));
	//Serial.print(UseCustomSelectKey ? F("Yes") : F("No "));
	//Serial.print(F("   Press: "));
	//Serial.print(SelectPressed ? F("Yes") : F("No "));
	//Serial.println();
}

TASK_CLASS_END;

bool RegisterLcdInfoTask(Scheduler & scheduler)
{
	return scheduler.Register(Instance<LcdInfoTask>());
}
