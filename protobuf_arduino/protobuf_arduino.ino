
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"

#include "peachduino.h"

// #define _sclk 13
// #define _miso 12
// #define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8




Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);

PeachDuino* peachDuino = new PeachDuino(Serial);
int loops = 0;

void setup() {
  Serial.begin(9600);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9340_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9340_WHITE);
  tft.setTextSize(2);
  tft.println("Initialization");
  displaySetup();
}

void displaySetup()
{
  tft.setCursor(0, 60);
  tft.println("lucky number");

  tft.setCursor(0, 80);
  tft.println("fails");

  tft.setCursor(0, 100);
  tft.println("success");

  tft.setCursor(0, 140);
  tft.println("bytes <-");

  tft.setCursor(0, 160);
  tft.println("bytes ->");

  tft.setCursor(0, 180);
  tft.println("runloops");
}

void display() {
  tft.fillRect(0, 0, 320, 15, 0x0000);
  tft.setCursor(0, 0);
  tft.println(peachDuino->message.message);

  tft.fillRect(160, 60, 45, 15, 0x0000);
  tft.setCursor(160, 60);
  tft.println(peachDuino->message.lucky_number);

  tft.fillRect(160, 80, 45, 15, ILI9340_RED);
  tft.setCursor(160, 80);
  tft.println(peachDuino->fails());

  tft.fillRect(160, 100, 45, 15, 0x04B0);
  tft.setCursor(160, 100);
  tft.println(peachDuino->success());

  tft.fillRect(160, 140, 55, 15, 0x28BB);
  tft.setCursor(160, 140);
  tft.println(peachDuino->recieved);

  tft.fillRect(160, 160, 55, 15, 0x2DBB);
  tft.setCursor(160, 160);
  tft.println(peachDuino->sent);

  tft.fillRect(160, 180, 160, 15, 0x46B0);
  tft.setCursor(160, 180);
  tft.println(loops);
}

void update() {
  display();
  peachDuino->sendMessage();
}

void loop(void) {
  loops++;
  peachDuino->process();
  if (loops % 100 == 0){
      update();
  }
}



