#include "Startup.h"

#include "LcdIo.h"
#include "Globals.h"

void ShowStartCountdown()
{
	lcdPrint(F("Select mode"));

	// �������� ������ � ��������� ���������� D13 (���������� �� �����, ����� ��������������).
	pinMode(Pins::LED_WARNING, OUTPUT);
	char buffer[17] = {};
	for (int i = 0; i < 16; ++i)
	{
		buffer[i] = '#';
		lcdPrint(0, buffer, false);
		digitalWrite(Pins::LED_WARNING, 1);
		delay(250);
		digitalWrite(Pins::LED_WARNING, 0);
		delay(250);
	}
}

byte ReadStartupMode()
{
	// ��������� �������������� ���������, �������� ������������ �������
	pinMode(Pins::MODE_SELECTOR, INPUT);
	digitalWrite(Pins::MODE_SELECTOR, 1);
	delay(10);
	return digitalRead(Pins::MODE_SELECTOR);
}
