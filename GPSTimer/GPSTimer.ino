#include "GlobalConfig.h"
#include "LcdInfoTask.h"
#include "ButtonsReaderTask.h"
#include <EEPROM.h>  // для работы с EEPROM
#include <LiquidCrystal.h>

#include "DataRecord.h"

#include "Globals.h"
#include "Startup.h"
#include "LcdIo.h"
#include "EepromUnload.h"
#include "OperatingMode.h"


/*** Реализация ***/

void setup() __ATTR_NORETURN__;

void setup() {
	lcdInit();

	/*fv */
    lcdPrint(F("Firm " __DATE__), F("Ver   " __TIME__));
	delay(5000);

	ShowStartCountdown();


	// если сигнал - 1, то подключения к земле нет - переходим в операционный режим
	if (ReadStartupMode() == 1)
	{
		// конфигурация скорости передачи - 9600 бод
		NMEA.begin(9600);
		if (&NMEA != &ECHO) 
		{ // если используются разные порты, то для связи с компьютером использовать высокую скорость
			ECHO.begin(115200);
		}
		PerformOperating();
	}
	else 
	{
		ECHO.begin(115200);
		PerformUnload();
	}
	for (;;);
}

void loop() {}

