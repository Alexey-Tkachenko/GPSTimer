#ifndef _WRITE_QUEUE_H_
#define _WRITE_QUEUE_H_

#include "Assert.h"

extern HardwareSerial& ECHO;

template<class T, typename TPtr = byte, TPtr Size = 16>
class WriteQueue
{
	// объект очереди
	T queue[Size];

	// указатели чтения-записи буферов.
	TPtr qRead;

	volatile TPtr qWrite;

public:
	WriteQueue() : qRead(), qWrite()
	{}
	~WriteQueue() {}

	T& Allocate()
	{
		return queue[qWrite];
	}

	void Complete(T& ref)
	{
		ASSERT(&ref == &queue[qWrite]);

		++qWrite;
		if (qWrite >= Size)
		{
			qWrite -= Size;
		}
	}

	bool HasData() const
	{
		return (qRead != qWrite);
	}

	T& Peek()
	{
		return queue[qRead];
	}

	void Free(T& ref)
	{
		ASSERT(&ref == &queue[qRead]);
		
		++qRead;
		if (qRead >= Size)
		{
			qRead -= Size;
		}
	}

	void Reset()
	{
		qRead = qWrite = 0;
	}
};

#endif
