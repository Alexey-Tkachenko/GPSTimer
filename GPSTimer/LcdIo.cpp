#include "LcdIo.h"
#include <LiquidCrystal.h>

#define DISPLAY_RS 5
#define DISPLAY_EN 6
#define DISPLAY_D4 9
#define DISPLAY_D5 10
#define DISPLAY_D6 11
#define DISPLAY_D7 12

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const byte LCD_ROWS = 2;
const byte LCD_COLS = 16;

void lcdClear()
{
	lcd.clear();
}

void lcdClearLine(byte line)
{
	lcd.setCursor(0, line);
	for (byte i = 0; i < LCD_COLS; ++i)
	{
		lcd.write(' ');
	}
}

void lcdInit()
{
	lcd.begin(16, 2);

	byte c[8];
	memset(c, 0, 8);

	c[0] = B00110;
	c[1] = B01001;
	c[2] = B01001;
	c[3] = B00110;
		
	lcd.createChar(SYMBOL_DEG, c);

	c[0] = B00010;
	c[1] = B00010;
	c[2] = B00100;
	c[3] = B00100;
	lcd.createChar(SYMBOL_MIN, c);

	c[0] = B01001;
	c[1] = B01001;
	c[2] = B10010;
	c[3] = B10010;
	lcd.createChar(SYMBOL_SEC, c);

	c[0] = B00111;
	c[1] = B00100;
	c[2] = B00100;
	c[3] = B01110;
	c[4] = B10101;
	c[5] = B00100;
	c[6] = B00100;
	c[7] = B11100;
	lcd.createChar(SYMBOL_RISE, c);

	c[0] = B11100;
	c[1] = B00100;
	c[2] = B00100;
	c[3] = B10101;
	c[4] = B01110;
	c[5] = B00100;
	c[6] = B00100;
	c[7] = B00111;
	lcd.createChar(SYMBOL_FALL, c);


}

void lcdPrint(const char* a, const char* b, bool clear)
{
	if (clear)
	{
		lcdClear();
	}

	if (a)
	{
		lcd.setCursor(0, 0);
		lcd.print(a);
	}

	if (b)
	{
		lcd.setCursor(0, 1);
		lcd.print(b);
	}
}

void lcdPrint(const __FlashStringHelper *a, const __FlashStringHelper *b, bool clear)
{
	if (clear)
	{
		lcdClear();
	}

	if (a)
	{
		lcd.setCursor(0, 0);
		lcd.print(a);
	}

	if (b)
	{
		lcd.setCursor(0, 1);
		lcd.print(b);
	}
}

void lcdPutCharAt(byte row, byte col, char symbol)
{
	lcd.setCursor(col, row);
	lcd.write(symbol);
}

void lcdPutLineAt(byte row, byte col, const char* symbols)
{
	lcd.setCursor(col, row);
	lcd.print(symbols);
}

void lcdPutLineAt(byte row, byte col, const __FlashStringHelper* symbols)
{
	lcd.setCursor(col, row);
	lcd.print(symbols);
}
