#include "MessageProcessor.h"
#include "SnakeLights.h"

String Empty = "";
String ConnectedMessage = "C";
const char ClearMessage = 'X';
const char SyncMessagePrefix = 'S';

String actives [256];
String hash = "#";
String comma = ",";

void message_processor::clear_state()
{
    for (int i = 0; i < 256; i++) {
      actives[i] = "";
    }
    mqtt_->publish("X");
}

void message_processor::process(String& topic, String& framedata)
{ 
  Serial.println("MQTT Message Received. Topic: " + topic);
  Serial.println("Message: " + framedata);
  
  if (framedata == Empty) return;
  if (framedata[0] == ClearMessage) return;
  if (framedata[0] == SyncMessagePrefix) return;
  if (framedata == ConnectedMessage)
  {    
    Serial.println(F("Another client connected! Sending sync message."));
    
    String message = "";
    for (int i = 0; i < 256; i++) {
      String hexCode = actives[i];      

      if (hexCode != "") {
        String istr = String(i);
        message += istr + hash + hexCode + comma;
      }
    }

    auto success = mqtt_->publish("S:" + message);
    if (!success)
    {
      Serial.println("Could not send sync message.");
    }

    return;
  }

  // Example = 100#FFFFFF
  const auto pixel_end = framedata.indexOf('#', 0);    
  const auto pixel_number_str = framedata.substring(0, pixel_end);
  const auto hex_colour_str = framedata.substring(pixel_end + 1);  
  auto pixel_number = pixel_number_str.toInt();  

  actives[pixel_number] = hex_colour_str;
  
  char hex_code[7];
  int r, g, b = 0;    
  hex_colour_str.toCharArray(hex_code, 7);  
  sscanf(hex_code, "%02x%02x%02x", &r, &g, &b);
  
  snake_lights::update_pixel(pixel_number, r, g, b);  
}