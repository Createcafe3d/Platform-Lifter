
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

#define HEADER 0x00
#define FOOTER 0xFF
#define ESCAPE 0xFE


Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);

uint8_t inputBuffer[512];
uint8_t readBuffer[64];
uint8_t bufferPos = 1;
bool status;
bool escaped;
Simple message = Simple_init_zero;
int available = 0;
int fails = 0;
int success = 0;
bool updateRequired = false;

void setup() {
  Serial.begin(14400);
  while (!Serial);
  // Serial.println("Initializtion Stating"); 
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9340_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9340_WHITE);
  tft.setTextSize(1);
  tft.println("Initialization");
  
}

void display() {
  tft.fillRect(0, 0, 160, 10, 0x0000);
  tft.setCursor(0, 0);
  tft.println(message.message);

  tft.fillRect(0, 60, 30, 10, 0x0000);
  tft.setCursor(0, 60);
  tft.println(message.lucky_number);

  tft.fillRect(0, 70, 30, 10, ILI9340_RED);
  tft.setCursor(0, 70);
  tft.println(fails);

  tft.fillRect(0, 80, 30, 10, ILI9340_GREEN);
  tft.setCursor(0, 80);
  tft.println(success);
}

void decode(uint8_t *buffer, size_t message_length) {
  uint8_t TYPE_ID = buffer[0];
  pb_istream_t stream = pb_istream_from_buffer(buffer + 1, message_length - 1);
  status = pb_decode(&stream, Simple_fields, &message);
  if (!status)
  {
    fails++;
  } else {
    success++;
  }

}

void loop(void) {
  available = Serial.available();
  if (available) {
    Serial.readBytes(readBuffer, available);
    for (short i = 0; i < available; i++) {
      inputBuffer[bufferPos] = readBuffer[i];
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
            updateRequired = true;
            break;
          default:
            bufferPos++;
            break;
        }
      }
    }
  } else {
    if (updateRequired) {
      updateRequired = false;
      display();
    }
  }
}



