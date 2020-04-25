#ifndef _MESSAGEPROCESSOR_h
#define _MESSAGEPROCESSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "MqttConnection.h"

class message_processor
{
	public:
		void clear_state();
		void process(String&, String&);
		
		message_processor(mqtt_connection* mqtt) {
			mqtt_ = mqtt;
		}

	protected:
		mqtt_connection* mqtt_ = nullptr;	
};

#endif
