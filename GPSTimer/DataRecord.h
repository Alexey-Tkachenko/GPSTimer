#pragma once
#ifndef _DATA_RECORD_H_
#define _DATA_RECORD_H_

#include "Arduino.h"


// структура поля даты
union DateTime
{
	struct T {
		unsigned long year : 6;
		unsigned long month : 4;
		unsigned long day : 5;
		unsigned long hour : 5;
		unsigned long minute : 6;
		unsigned long second : 6;
	} t;

	byte raw[sizeof(T)];
};

// Поле расположения
struct Position
{
	float Latitude;
	float Longitude;
	int Altitude;
};

// Структура записи в EEPROM
struct Record
{
	::DateTime       Time; // базовые дата-время

	::Position       Position; // положение, если разрешено

	unsigned long    Type : 1; // тип - 1 бит (0/1)
	unsigned long    Offset : 31; // смещение, остальные биты

	unsigned short   Check; // контрольное слово - проверяет целостность записи

	static const unsigned short OK = 0x4F6B;
	static const unsigned short ERASED = 0x4552;
};

#endif

