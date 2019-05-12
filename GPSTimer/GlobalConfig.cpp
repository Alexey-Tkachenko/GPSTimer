// 
// 
// 

#include "GlobalConfig.h"

#include <EEPROM.h>

#ifdef SAVE_GLOBAL_CONFIG

const int ConfigStorageOffset = EEPROM.length() - sizeof(GlobalConfig);

#endif

static GlobalConfig config;

static void LoadConfig();

GlobalConfig & Config()
{
#ifdef SAVE_GLOBAL_CONFIG
	static bool configLoaded = false;
	if (!configLoaded)
	{
		configLoaded = true;
		LoadConfig();
	}
#endif
	return config;
}

void LoadConfig()
{
#ifdef SAVE_GLOBAL_CONFIG
	EEPROM.get(ConfigStorageOffset, config);
#endif
}

void FlushConfig()
{
#ifdef SAVE_GLOBAL_CONFIG
	EEPROM.put(ConfigStorageOffset, config);
#endif
}
