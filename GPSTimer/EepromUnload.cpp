#include <EEPROM.h>  // для работы с EEPROM

#include "EepromUnload.h"
#include "Globals.h"
#include "DataRecord.h"
#include "LcdIo.h"
#include "SoundTask.h"
#include "GlobalConfig.h"

// сброс данных по через COM-интерфейс.
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

	// сформировать заголовок
	ECHO.print(F("#Offset\tY\tM\tD\th\tm\ts"));

	ECHO.print(F("\tlon\tlat\talt"));
	
	ECHO.print(F("\tT\tOfs"));
	ECHO.println();


	// пройтись по всем записям в памяти EEPROM
	for (int i = 0, end = EEPROM.length() - sizeof(Record) + 1; i < end; i += sizeof(Record)) {

		Record r;
		// извлечь текущую
		EEPROM.get(i, r);

		if (r.Check != Record::OK) {
			// если запись пуста или не дописана, перейти к следующей
			continue;
		}

		// вывести содержимое данных
		ECHO.write('#');
		ECHO.print(i);
		ECHO.print(tab);

		// ссылка-псевдоним для сокращения записи
		const DateTime::T& t = r.Time.t;

		// вывод даты-времени
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

		// вывод информации о прерывании
		ECHO.print(tab);
		ECHO.print(r.Type);
		ECHO.print(tab);
		ECHO.print(r.Offset * 1e-6, 6);
		ECHO.println();
	}

	ECHO.println(F("###"));
}

// очистка памяти
void EraseData()
{
#ifdef SAVE_GLOBAL_CONFIG
#define CFG_SIZE sizeof(GlobalConfig)
#else
#define CFG_SIZE 0
#endif
	// пройтись по всей EEPROM
	for (unsigned i = 0; i < EEPROM.length() - CFG_SIZE; ++i)
	{
		// и записать нули.
		EEPROM[i] = 0;
	}
}

void PerformUnload()
{
	lcdPrint(F("Mode: Download"));

	// собственно сброс
	DownloadData();

	lcdPrint(0, F("Reboot or Erase"), false);

	// дождаться, чтобы пользователь убрал замыкание на землю (если будет перезагрузка - то стирания не будет)
	while (digitalRead(Pins::MODE_SELECTOR) == 0);

	lcdPrint(F("Erasing EEPROM"));

	// включить индикацию
	digitalWrite(Pins::LED_WARNING, 1);

	// стереть данные
	EraseData();
	lcdPrint(F("EEPROM erased"), F("Reboot me"));

	// выключить индикацию
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
