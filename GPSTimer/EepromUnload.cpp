#include <EEPROM.h>  // ��� ������ � EEPROM

#include "EepromUnload.h"
#include "Globals.h"
#include "DataRecord.h"
#include "LcdIo.h"
#include "SoundTask.h"
#include "GlobalConfig.h"

// ����� ������ �� ����� COM-���������.
void DownloadData() {
	const char *tab = "\t";
	for (int i = 0, end = EEPROM.length(); i < end; ++i) 
	{
		if (i % sizeof(Record) == 0) 
		{
			ECHO.println();
		}
		byte v = EEPROM[i];
		if (v < 16) {
			ECHO.write('0');
		}
		ECHO.print(v, HEX);
		ECHO.write(' ');
	}
	ECHO.println();

	// ������������ ���������
	ECHO.print(F("#Offset\tY\tM\tD\th\tm\ts"));

	ECHO.print(F("\tlon\tlat\talt"));
	
	ECHO.print(F("\tT\tOfs"));
	ECHO.println();


	// �������� �� ���� ������� � ������ EEPROM
	for (int i = 0, end = EEPROM.length() - sizeof(Record) + 1; i < end; i += sizeof(Record)) {

		Record r;
		// ������� �������
		EEPROM.get(i, r);

		if (r.Check != Record::OK) {
			// ���� ������ ����� ��� �� ��������, ������� � ���������
			continue;
		}

		// ������� ���������� ������
		ECHO.write('#');
		ECHO.print(i);
		ECHO.print(tab);

		// ������-��������� ��� ���������� ������
		const DateTime::T& t = r.Time.t;

		// ����� ����-�������
		ECHO.print(t.year);
		ECHO.print(tab);
		ECHO.print(t.month);
		ECHO.print(tab);
		ECHO.print(t.day);
		ECHO.print(tab);
		ECHO.print(t.hour);
		ECHO.print(tab);
		ECHO.print(t.minute);
		ECHO.print(tab);
		ECHO.print(t.second);

		ECHO.print(tab);
		ECHO.print(r.Position.Longitude, 7);
		ECHO.print(tab);
		ECHO.print(r.Position.Latitude, 7);
		ECHO.print(tab);
		ECHO.print(r.Position.Altitude, 1);

		// ����� ���������� � ����������
		ECHO.print(tab);
		ECHO.print(r.Type);
		ECHO.print(tab);
		ECHO.print(r.Offset * 1e-6, 6);
		ECHO.println();
	}

	ECHO.println(F("###"));
}

// ������� ������
void EraseData()
{
#ifdef SAVE_GLOBAL_CONFIG
#define CFG_SIZE sizeof(GlobalConfig)
#else
#define CFG_SIZE 0
#endif
	// �������� �� ���� EEPROM
	for (unsigned i = 0; i < EEPROM.length() - CFG_SIZE; ++i)
	{
		// � �������� ����.
		EEPROM[i] = 0;
	}
}

void PerformUnload()
{
	lcdPrint(F("Mode: Download"));

	// ���������� �����
	DownloadData();

	lcdPrint(0, F("Reboot or Erase"), false);

	// ���������, ����� ������������ ����� ��������� �� ����� (���� ����� ������������ - �� �������� �� �����)
	while (digitalRead(Pins::MODE_SELECTOR) == 0);

	lcdPrint(F("Erasing EEPROM"));

	// �������� ���������
	digitalWrite(Pins::LED_WARNING, 1);

	// ������� ������
	EraseData();
	lcdPrint(F("EEPROM erased"), F("Reboot me"));

	// ��������� ���������
	digitalWrite(Pins::LED_WARNING, 0);
}

bool CheckMemoryFull()
{
	if (WriteOffset + sizeof(Record) > EEPROM.length())
	{
		return true;
	}
	return false;
}
