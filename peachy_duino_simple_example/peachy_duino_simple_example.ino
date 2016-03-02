#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"
#include "messages.pb.h"

#include "simplepeachduino.h"

// #define _sclk 13
// #define _miso 12
// #define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8




Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);

SimplePeachDuino* peachDuino = new SimplePeachDuino(Serial);
unsigned long loops = 0;
unsigned long drips = 0;
unsigned long lastHeight = 0;

void setup() {

  attachInterrupt(digitalPinToInterrupt(2), incAndSendDrips, RISING);
  Serial.begin(9600);

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9340_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9340_WHITE);
  tft.setTextSize(2);
  displaySetup();
}

void displaySetup(){
  tft.setCursor(0, 0);
  tft.println("Current Height");

  tft.setCursor(0, 20);
  tft.println("Target Height");

  tft.setCursor(0, 40);
  tft.println("Waiting for Drips");

  tft.setCursor(0, 60);
  tft.println("Status");

  tft.setCursor(0, 100);
  tft.println("Drips");

  tft.setCursor(0, 120);
  tft.println("Height");

  tft.setCursor(0, 140);
  tft.println("Fails");

  tft.setCursor(0, 160);
  tft.println("Success");

  tft.setCursor(0, 180);
  tft.println("bytes <-");

  tft.setCursor(0, 200);
  tft.println("bytes ->");

  tft.setCursor(0, 220);
  tft.println("runloops");
}

void display() {
  tft.fillRect(280, 0, 40, 15, 0x0000);
  tft.setCursor(280, 0);
  tft.println(peachDuino->targetHeightMicrometer);

  tft.fillRect(280, 20, 40, 15, 0x0000);
  tft.setCursor(280, 20);
  tft.println(peachDuino->currentHeightMicrometer);

  tft.fillRect(280, 40, 40, 15, 0x0000);
  tft.setCursor(280, 40);
  tft.println(peachDuino->waitingForNextLayerHeight);

  tft.fillRect(280, 60, 40, 15, 0x0000);
  tft.setCursor(280, 60);
  tft.println(peachDuino->printStatus);

  tft.fillRect(160, 100, 45, 15, 0x0000);
  tft.setCursor(160, 100);
  tft.println(drips);

  tft.fillRect(160, 120, 50, 15, 0x0000);
  tft.setCursor(160, 120);
  tft.println(lastHeight);

  tft.fillRect(160, 140, 45, 15, ILI9340_RED);
  tft.setCursor(160, 140);
  tft.println(peachDuino->fails());

  tft.fillRect(160, 160, 45, 15, 0x04B0);
  tft.setCursor(160, 160);
  tft.println(peachDuino->success());

  tft.fillRect(160, 180, 120, 15, 0b1111100000000000);
  tft.setCursor(160, 180);
  tft.println(peachDuino->recieved());

  tft.fillRect(160, 200, 120, 15, 0b0000001111000000);
  tft.setCursor(160, 200);
  tft.println(peachDuino->sent());

  tft.fillRect(160, 220, 120, 15, 0b0000000000011111);
  tft.setCursor(160, 220);
  tft.println(loops);
}

void incAndSendDrips() {
  drips++;
  peachDuino->sendDripCount(drips);
}

void update() {
  display();
}

void loop(void) {
  loops++;
  

  if (loops % 1000 == 0)
  {
    if (lastHeight != analogRead(0))
    {
      lastHeight = analogRead(0);
      peachDuino->sendHeightMicrometer(lastHeight);
    }
  peachDuino->process();
    update();
  }
}



