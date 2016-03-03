//--------------Example to Demostrate Basic Functionality of API-------------
//  Circuit:  [ https://123d.circuits.io/circuits/1702716-status-light-example ]
//  


#include "simplepeachduino.h"

#define NEW_LED 4
#define PRINTING_LED 5
#define WAITING_LED 6
#define FINISHED_LED 7

SimplePeachDuino* peachDuino = new SimplePeachDuino(Serial);

void setup() {
  Serial.begin(9600);
  
  pinMode(NEW_LED, OUTPUT);
  pinMode(PRINTING_LED, OUTPUT);
  pinMode(WAITING_LED, OUTPUT);
  pinMode(FINISHED_LED, OUTPUT);

  digitalWrite(NEW_LED, HIGH);
  digitalWrite(PRINTING_LED, HIGH);
  digitalWrite(WAITING_LED, HIGH);
  digitalWrite(FINISHED_LED, HIGH);
}

void loop(void) {
  peachDuino->process();
  switch(peachDuino->printStatus)
  {
    case 0:
      digitalWrite(NEW_LED, LOW);
      digitalWrite(PRINTING_LED, HIGH);
      digitalWrite(WAITING_LED, HIGH);
      digitalWrite(FINISHED_LED, HIGH);
    case 1:
      digitalWrite(NEW_LED, HIGH);
      digitalWrite(PRINTING_LED, LOW);
      digitalWrite(WAITING_LED, HIGH);
      digitalWrite(FINISHED_LED, HIGH);
    case 2:
      digitalWrite(NEW_LED, HIGH);
      digitalWrite(PRINTING_LED, HIGH);
      digitalWrite(WAITING_LED, LOW);
      digitalWrite(FINISHED_LED, HIGH);
    case 3:
      digitalWrite(NEW_LED, HIGH);
      digitalWrite(PRINTING_LED, HIGH);
      digitalWrite(WAITING_LED, HIGH);
      digitalWrite(FINISHED_LED, LOW);
    default:
      digitalWrite(NEW_LED, HIGH);
      digitalWrite(PRINTING_LED, HIGH);
      digitalWrite(WAITING_LED, HIGH);
      digitalWrite(FINISHED_LED, HIGH);
  }
  
}



