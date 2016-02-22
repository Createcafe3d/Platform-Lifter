
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"
#include <pb_encode.h>
#include <pb_decode.h>

// #define _sclk 13
// #define _miso 12
// #define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8

Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Initializtion Stating"); 
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9340_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9340_WHITE);
  tft.setTextSize(1);
  tft.println("Initialization");
}

void loop(void) {
    if (Serial.available() > 0) {
      tft.println(Serial.read(), HEX);
    }
}


