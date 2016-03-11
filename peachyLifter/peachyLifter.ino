#include "Arduino.h"
#define ledPin 7

//16MHz clock, with prescaler of 64
//To get a "tick" time of 200us we do:
// <time per tick> * <Clock frequency> / <prescaler>
// 200e-6 * 16e6 / 64 = 50
float cpu_freq = 16e6;
float tick_time = 200e-6;
uint8_t prescaler = 64;
int8_t tim2_start = 256-tick_time*cpu_freq/prescaler;
//tim2_start = -50; note negative works here due to 2's compliment. (-50 == 206) in the 8 bit data types

uint16_t interrupt_count=0;

void setup()
{
	Serial.begin(115200);
	pinMode(12,INPUT_PULLUP); 
	pinMode(ledPin,OUTPUT); 
	noInterrupts();
	setupTIM1_ISR();
  printSetups();
	//interrupt setups here
  interrupts();
}

void printSetups(){
  Serial.print("tim2_start is:");
  Serial.println(tim2_start);
}

void setupTIM1_ISR(){
	//Register definitions found Page ~131 in datasheet
	//TCNT2   //Timer/Counter Register. The actual timer value is stored here.
  //TCCR2  //Timer Control Register (Prescaler here)
	//TIMSK   //Timer Interrupt Mask Register
 
  //Prescaler is set as follows [CS22, CS21, CS20]:
  // 000 - Stopped
  // 001 - no prescaler
  // 010 - divide by 8
  // 011 - divide by 32
  // 100 - divide by 64
  // 101 - divide by 128
  // 110 - divide by 256
  // 111 - divide by 1024
  TCCR2B |= ((1 << CS22) | (0 << CS21) | (0 << CS20)); //64
  //Timer2 Overflow Interrupt Enable
  TIMSK2 |= (1 << TOIE2);
  TCNT2=tim2_start;

}

ISR(TIMER2_OVF_vect){
  TCNT2=tim2_start; //Reset the timer to start value for consistant roll overs
	digitalWrite(ledPin, digitalRead(ledPin) ^ 1); //Toggle LED on each interrupt cycle
  interrupt_count++;

  //Flag cleared automagically
}

void loop()
{
  if (interrupt_count>5000){
    Serial.print("Interrupt Count=");
    Serial.println(interrupt_count);
    interrupt_count=0;
  }
  int bar = 0;

}

