#ifndef _SNAKELIGHTS_h
#define _SNAKELIGHTS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class snake_lights
{
public:
	static void init();
  static void device_ready_light();
	static void update_pixel(int pixel, int r, int g, int b);
};

#endif
