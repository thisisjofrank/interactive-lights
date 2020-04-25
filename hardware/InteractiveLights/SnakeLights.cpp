#include <Adafruit_NeoPixel.h>
#include "SnakeLights.h"

#if defined(ESP32)
#define PIN 21
#else
#define PIN 4
#endif

#define NUM_LIGHTS  256

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LIGHTS, PIN, NEO_GRB + NEO_KHZ800);

void snake_lights::init()
{
	strip.begin();
	strip.show();

  // Top left set to red while booting.
  strip.setPixelColor(0, Adafruit_NeoPixel::Color(255, 0, 0));
  for (auto i = 1; i < NUM_LIGHTS; i++) { strip.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 0)); }
}

void snake_lights::update_pixel(int pixel, int r, int g, int b)
{
  auto y = pixel / 16;
  auto shouldSnake = y % 2 == 0;
  auto lineOffset = y * 16;
  
  auto regularX = (pixel % 16);
  auto snakeX = (15 - regularX);
  
  auto x = shouldSnake ? snakeX : regularX;
  auto snakePixelId = x + (lineOffset);

	Serial.print(F("Converting requested pixel "));
	Serial.print(pixel);
	Serial.print(F(" to snaked pixel "));
	Serial.print(snakePixelId);
	Serial.println("");

	Serial.println(F("Updating lights."));

  Serial.print(snakePixelId);
  Serial.print(F(":("));
  Serial.print(r);
  Serial.print(F(","));
  Serial.print(g);
  Serial.print(F(","));
  Serial.print(b);
  Serial.print(F(")\r\n"));

  strip.setPixelColor(snakePixelId, r, g, b);
	strip.show();

	Serial.println(F("Lights set"));
}

void snake_lights::device_ready_light()
{
  strip.setPixelColor(0, Adafruit_NeoPixel::Color(0, 255, 0));
  for (auto i = 1; i < NUM_LIGHTS; i++) { strip.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 0)); }
}
