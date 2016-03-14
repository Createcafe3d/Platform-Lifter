#include "Arduino.h"

//g_Flagger and g_Stepper are found in PeachyTimer2Interrupt.h
#include "PeachyTimer2Interrupt.h"

#define ledPin 7

uint8_t g_1000ms_flag = g_Flagger.registerFlag(5000);
uint8_t rylan_do_next_second=0;

void setup()
{
	Serial.begin(115200);
	pinMode(12,INPUT_PULLUP); 
	pinMode(ledPin,OUTPUT); 

	noInterrupts();
	setupTIM2_ISR();
  printSetups();
  interrupts();
}

void printSetups(){
  Serial.print("tim2_start is:");
  Serial.println(TIM2_START);
  Serial.print("prescaler is:");
  Serial.println(TIM2_PRESCALER);
}

void loop()
{
  uint8_t move_direction;
  uint16_t tmp_count;
  
  if (g_interrupt_count>5000){
    tmp_count=g_interrupt_count;
    Serial.print("Interrupt Count=");
    Serial.println(tmp_count);
    g_interrupt_count=0;
  }

	//This happens once a second
	if (g_Flagger.getFlag(g_1000ms_flag)){
    Serial.println("ONE SECOND");
    g_Flagger.clearFlag(g_1000ms_flag);
    move_direction=digitalRead(ledPin);
    g_Stepper.move(move_direction,200);
		digitalWrite(ledPin, move_direction ^ 1); //Toggle LED

		
	}
}

