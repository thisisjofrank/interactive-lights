#ifndef _NETWORKING_h
#define _NETWORKING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class networking
{
 public:
	static void ensure_wifi_connected(const char* const ssid, const char* const password);
};

#endif
