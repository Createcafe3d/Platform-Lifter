
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

#define SIMPLETYPE 0x01;


Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);

uint8_t inputBuffer[128];
uint8_t outputBuffer[128];
uint8_t readBuffer[64];
uint8_t bufferPos = 1;
bool escaped;
Simple message = Simple_init_zero;
int available = 0;
int fails = 0;
int success = 0;
bool updateRequired = true;
int cnt = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  // Serial.println("Initializtion Stating"); 
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9340_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9340_WHITE);
  tft.setTextSize(2);
  tft.println("Initialization");
  
}

void display() {
  tft.fillRect(0, 0, 160, 15, 0x0000);
  tft.setCursor(0, 0);
  tft.println(message.message);

  tft.fillRect(0, 60, 45, 15, 0x0000);
  tft.setCursor(0, 60);
  tft.println(message.lucky_number);

  tft.fillRect(0, 80, 45, 15, ILI9340_RED);
  tft.setCursor(0, 80);
  tft.println(fails);

  tft.fillRect(0, 100, 45, 15, 0x04B0);
  tft.setCursor(0, 100);
  tft.println(success);
}

void decode(uint8_t *buffer, size_t message_length) {
  uint8_t TYPE_ID = buffer[0];
  pb_istream_t stream = pb_istream_from_buffer(buffer + 1, message_length - 1);
  bool status = pb_decode(&stream, Simple_fields, &message);
  if (!status)
  {
    fails++;
  } else {
    success++;
  }
}


void sendMessage() {
  Simple message = {success, "Success"};
  pb_ostream_t stream = pb_ostream_from_buffer(outputBuffer, sizeof(outputBuffer));
  bool status = pb_encode(&stream, Simple_fields, &message);
  uint8_t encodedBuffer[128];
  encodedBuffer[0] = HEADER;
  encodedBuffer[1] = SIMPLETYPE;
  int encodedBufferIndex = 2;
  for (int outputBufferIndex = 0; outputBufferIndex < stream.bytes_written; outputBufferIndex++){
    switch(outputBuffer[outputBufferIndex]){
      case ESCAPE:
        encodedBuffer[encodedBufferIndex] = ESCAPE;
        encodedBufferIndex++;
        break;
      case HEADER:
        encodedBuffer[encodedBufferIndex] = ESCAPE;
        encodedBufferIndex++;
        break;
      case FOOTER:
        encodedBuffer[encodedBufferIndex] = ESCAPE;
        encodedBufferIndex++;
        break;
    }
    encodedBuffer[encodedBufferIndex] = outputBuffer[outputBufferIndex];
    encodedBufferIndex++;
  }
  encodedBuffer[encodedBufferIndex] = FOOTER;
  Serial.write(encodedBuffer, encodedBufferIndex + 1);
}

bool readSerial() {
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
    return true;
  } else {
    return false;
  }
}

void update() {
  updateRequired = false;
  display();
  sendMessage();
  delay(1000);
}

void loop(void) {
  if (!readSerial()) {
    if (updateRequired) {
      update();
    }
  }
}



