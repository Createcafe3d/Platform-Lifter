#include "Arduino.h"
#include <stdint.h>

#define STEPPER_EN_PIN 6
#define STEPPER_DIR_PIN 7
#define STEPPER_STEP_PIN 8
#define LED1_PIN 10 
#define LED2_PIN 9 

#define UP 1
#define DOWN 0

#define STEP_DELAY 2

uint8_t g_direction=UP;

void stepperStep(){
	digitalWrite(STEPPER_DIR_PIN,g_direction);
	digitalWrite(STEPPER_STEP_PIN,1);
	delay(STEP_DELAY);
	digitalWrite(STEPPER_STEP_PIN,0);
}

void stepperEnable(uint8_t state=1){
	digitalWrite(STEPPER_STEP_PIN,state);
}

void setup(){
	pinMode(STEPPER_EN_PIN,OUTPUT);
	pinMode(STEPPER_DIR_PIN,OUTPUT);
	pinMode(STEPPER_STEP_PIN,OUTPUT);
	pinMode(LED1_PIN,OUTPUT);
	pinMode(LED2_PIN,OUTPUT);
	stepperEnable();
}

void loop(){

	digitalWrite(LED2_PIN,0);
	digitalWrite(LED1_PIN,1);
	delay(50);
	g_direction=UP;
	for (uint16_t i=0;i<100;i++){
		stepperStep();
		delay(STEP_DELAY);
	}

	delay(50);
	g_direction=DOWN;
	digitalWrite(LED1_PIN,0);
	digitalWrite(LED2_PIN,1);
	for (uint16_t i=0;i<100;i++){
		stepperStep();
		delay(STEP_DELAY);
	}


}
