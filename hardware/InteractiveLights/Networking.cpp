#if defined(ESP32)
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include "DataStructures.h"
#include "Networking.h"

auto networking::ensure_wifi_connected(const char* const ssid, const char* const password) -> void
{
	if (WiFi.status() == WL_CONNECTED)
	{		
		return;
	}

	Serial.println(F("Connecting to "));
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(100);
		Serial.print(".");
	}

	Serial.println(F("WiFi connected"));
}
