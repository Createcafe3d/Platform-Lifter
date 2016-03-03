//--------------Example to Demostrate Basic Functionality of API-------------
//  Goal: use a servo to open and close valve
//  Limitations: ON/OFF there are of course advantages to partial open and close not covered in this example
//  Circuit:  [ https://123d.circuits.io/circuits/1702716-dripper-valve-example ]
//  


#include "simplepeachduino.h"
#include <Servo.h>

#define VALVE_PWM 3
#define DRIPS_REQUIRED 4
#define DRIPS_DRIPS_CORRECT 5

SimplePeachDuino* peachDuino = new SimplePeachDuino(Serial);
Servo dripperServo;

void setup() {
  Serial.begin(9600);
  dripperServo.attach(VALVE_PWM);

  pinMode(DRIPS_REQUIRED, OUTPUT);
  pinMode(DRIPS_DRIPS_CORRECT, OUTPUT);

  digitalWrite(DRIPS_REQUIRED, HIGH);
  digitalWrite(DRIPS_DRIPS_CORRECT, HIGH);

}

void loop(void) {
  peachDuino->process();
  if(peachDuino->targetHeightMicrometer > peachDuino->currentHeightMicrometer)
  {
    digitalWrite(DRIPS_REQUIRED, LOW);
    digitalWrite(DRIPS_DRIPS_CORRECT, HIGH);
    dripperServo.write(0);
  } else {
    digitalWrite(DRIPS_REQUIRED, HIGH);
    digitalWrite(DRIPS_DRIPS_CORRECT, LOW);
    dripperServo.write(180);
  }
  
}



