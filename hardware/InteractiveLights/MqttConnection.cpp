#if defined(ESP32)
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include "MqttConnection.h"
#include "DataStructures.h"
#include <WiFiClientSecure.h>
#include <MQTT.h>

WiFiClientSecure espClient;
MQTTClient client(4096);

void mqtt_connection::ensure_mqtt_connected(void(*callback)(String&, String&))
{ 
  if (client.connected()) {
    return;
  }
    
  espClient.setFingerprint(cfg_->mqtt.certificate);
  client.begin(cfg_->mqtt.server, cfg_->mqtt.port, espClient);
  client.onMessage(callback);

  while (!client.connected()) 
  {    
    Serial.print(F("Attempting MQTT connection..."));

    if (client.connect("tshirtHardware", cfg_->mqtt.user, cfg_->mqtt.password)) 
    {
      Serial.println(F("Connected."));
      client.subscribe(cfg_->mqtt.subscription);
    } 
    else 
    {
      Serial.println(F("Failed to connect to MQTT - Trying again in 5 seconds..."));
      delay(5000);
    }
  }
  
}

void mqtt_connection::process_messages()
{ 
	client.loop();
}

int mqtt_connection::publish(String message) 
{
	return client.publish(cfg_->mqtt.subscription, message);
}
