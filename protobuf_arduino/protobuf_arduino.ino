
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "messages.pb.h"

// #define _sclk 13
// #define _miso 12
// #define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8

#define HEADER 0x42
#define FOOTER 0x41
#define ESCAPE 0x40


Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);
uint8_t inputBuffer[256];
uint8_t bufferPos = 1;
bool status;
bool escaped;

void setup() {
  Serial.begin(192000);
  while (!Serial);
  // Serial.println("Initializtion Stating"); 
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9340_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9340_WHITE);
  tft.setTextSize(1);
  tft.println("Initialization");
  
}

int decode(uint8_t buffer[128], size_t message_length) {
  Simple message = Simple_init_zero;
  pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);
  status = pb_decode(&stream, Simple_fields, &message);
  if (!status)
  {
      tft.fillRect(0, 120, 160, 120, ILI9340_RED);
      return 1;
  }
  tft.fillRect(0, 120, 160, 40, 0x5555);
  tft.setCursor(0, 120);
  tft.println(message.lucky_number);
  tft.fillRect(0, 0, 160, 40, 0x0000);
  tft.setCursor(0, 0);
  tft.println(message.message);
  return 0;
}

void loop(void) {
    if (Serial.available()) {
      inputBuffer[bufferPos] = Serial.read();
      if (escaped) {
          bufferPos++;
          escaped = false;
      } else {
        switch(inputBuffer[bufferPos]) {
          case ESCAPE:
            escaped = true;
            break;
          case HEADER:
            bufferPos = 0;
            break;
          case FOOTER:
            decode(inputBuffer, bufferPos);
            break;
          default:
            bufferPos++;
            break;
            
        }
      }
    }
}



