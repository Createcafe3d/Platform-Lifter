#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"
#include "messages.pb.h"

#include "peachduino.h"

// #define _sclk 13
// #define _miso 12
// #define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8




Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);

PeachDuino* peachDuino = new PeachDuino(Serial);
PrinterStatus printerStatusMessage;
bool messageRefreshed = false;
unsigned long loops = 0;
unsigned long drips = 0;

void setup() {
  peachDuino->addHandler(51, printerStatusHandler);

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

  tft.setCursor(0, 80);
  tft.println("Drips");

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
  if (messageRefreshed){
    tft.fillRect(280, 0, 40, 15, 0x0000);
    tft.setCursor(280, 0);
    tft.println(printerStatusMessage.targetHeightMicrometer);

    tft.fillRect(280, 20, 40, 15, 0x0000);
    tft.setCursor(280, 20);
    tft.println(printerStatusMessage.currentHeightMicrometer);

    tft.fillRect(280, 40, 40, 15, 0x0000);
    tft.setCursor(280, 40);
    tft.println(printerStatusMessage.waitingForNextLayerHeight);

    tft.fillRect(280, 60, 40, 15, 0x0000);
    tft.setCursor(280, 60);
    tft.println(printerStatusMessage.status);

    messageRefreshed = false;
  }

  tft.fillRect(160, 80, 45, 15, 0x0000);
  tft.setCursor(160, 80);
  tft.println(drips);

  tft.fillRect(160, 140, 45, 15, ILI9340_RED);
  tft.setCursor(160, 140);
  tft.println(peachDuino->fails());

  tft.fillRect(160, 160, 45, 15, 0x04B0);
  tft.setCursor(160, 160);
  tft.println(peachDuino->success());

  tft.fillRect(160, 180, 120, 15, 0b1111100000000000);
  tft.setCursor(160, 180);
  tft.println(peachDuino->recieved);

  tft.fillRect(160, 200, 120, 15, 0b0000001111000000);
  tft.setCursor(160, 200);
  tft.println(peachDuino->sent);

  tft.fillRect(160, 220, 120, 15, 0b0000000000011111);
  tft.setCursor(160, 220);
  tft.println(loops);
}

void printerStatusHandler(void* newMessage) {
  printerStatusMessage = *((PrinterStatus*)newMessage);
  messageRefreshed = true;
}

void incAndSendDrips() {
  drips++;
  DripRecorded dripRecordedMessage = {drips};
  peachDuino->sendMessage(dripRecordedMessage);
}

void update() {
  display();
  // Simple message = {peachDuino->success(), "Success"};
  // peachDuino->sendMessage(message);
}

void loop(void) {
  loops++;
  peachDuino->process();

  if (loops % 1000 == 0){
      update();
  }
}



