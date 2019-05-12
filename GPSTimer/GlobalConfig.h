// GlobalConfig.h

#ifndef _GLOBALCONFIG_h
#define _GLOBALCONFIG_h

#include "Arduino.h"

struct GlobalConfig
{
	bool DisableSound : 1;
};

GlobalConfig& Config();

void FlushConfig();

struct ConfigLifeTime
{
	inline ~ConfigLifeTime() 
	{
		FlushConfig();
	}
};


#endif

