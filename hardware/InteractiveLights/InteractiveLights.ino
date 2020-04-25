#include "DataStructures.h"
#include "SnakeLights.h"
#include "MessageProcessor.h"
#include "MqttConnection.h"
#include "Networking.h"
#include <WiFiClientSecure.h>
#include <MQTT.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

int ticks = 0;
int interval = 250;

configuration cfg = {
  { "ilikepie", "Goldfish54!" },
  { 
    "mqtt.ably.io", 8883,    
    "api-key-part1", "api-key-part2", 
    "3b b1 e6 46 12 f9 f4 ac 53 59 f4 97 99 ee 35 c9 3b 3b 46 11", 
    "tshirt" 
  }
};

mqtt_connection* mqtt = new mqtt_connection(&cfg);
message_processor* processor = new message_processor(mqtt);

void process(String& topic, String& framedata) { processor->process(topic, framedata); }

void setup()
{
  snake_lights::init();

	Serial.begin(115200);
	delay(1000);

	networking::ensure_wifi_connected(cfg.wifi.ssid, cfg.wifi.password);
  snake_lights::device_ready_light();
}

void loop()
{    
  if (ticks == 80) {
    Serial.println("🚫🐓");
    ticks = 0;
    
    // Uncomment to enable auto-clearing!
    // processor->clear_state();
    // snake_lights::device_ready_light();
  }

  networking::ensure_wifi_connected(cfg.wifi.ssid, cfg.wifi.password);
  mqtt->ensure_mqtt_connected(process);  
  mqtt->process_messages();

  delay(interval);
  ticks++;
}