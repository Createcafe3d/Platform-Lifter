#include "Arduino.h"
#define ledPin 7

//16MHz timer, with prescaler of 1
//To get a "tick" time of 200us we do:
// 200e-6 * 16e6 = 3200
uint16_t tim1_countmax=3200;
uint16_t tim1_start=65535-tim1_countmax;
uint16_t interrupt_count=0;

void setup()
{
	Serial.begin(115200);
	pinMode(12,INPUT_PULLUP); 
	pinMode(ledPin,OUTPUT); 
	noInterrupts();
	setupTIM1_ISR();
	//interrupt setups here
  interrupts();
}
void setupTIM1_ISR(){
	//Register definitions found Page ~131 in datasheet
	//TCNT1   //Timer/Counter Register. The actual timer value is stored here.
  //TCCR1B  //Timer Control Register (Prescaler here)
	//TIMSK   //Timer Interrupt Mask Register

  //Example code:
  //Timer2 Settings:  Timer Prescaler /1024
  TCCR1A=0;
  TCCR1B=0;
  //Prescaler is set as follows [CS12, CS11, CS10]:
  // 000 - Stopped
  // 001 - no prescaler
  // 010 - divide by 8
  // 011 - divide by 64
  // 100 - divide by 256
  // 101 - divide by 1024
  TCCR1B |= ((0 << CS12) | (0 << CS11) | (1 << CS10)); //1
  //Timer2 Overflow Interrupt Enable
  TIMSK1 |= (1 << TOIE1);
  TCNT1=tim1_start;


}

ISR(TIMER1_OVF_vect){
  TCNT1=tim1_start; //Reset the timer to start value for consistant roll overs
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

