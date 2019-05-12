#pragma once
#ifndef _DATA_RECORD_H_
#define _DATA_RECORD_H_

#include "Arduino.h"


// ��������� ���� ����
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

// ���� ������������
struct Position
{
	float Latitude;
	float Longitude;
	int Altitude;
};

// ��������� ������ � EEPROM
struct Record
{
	::DateTime       Time; // ������� ����-�����

	::Position       Position; // ���������, ���� ���������

	unsigned long    Type : 1; // ��� - 1 ��� (0/1)
	unsigned long    Offset : 31; // ��������, ��������� ����

	unsigned short   Check; // ����������� ����� - ��������� ����������� ������

	static const unsigned short OK = 0x4F6B;
	static const unsigned short ERASED = 0x4552;
};

#endif

