#include "Arduino.h"
#include "Flagger.h"
#define ledPin 7

//16MHz clock, with prescaler of 64
//To get a "tick" time of 200us we do:
// <time per tick> * <Clock frequency> / <prescaler>
// 200e-6 * 16e6 / 64 = 50
#define CPU_FREQ 16e6 //MHz
#define TICK_TIME 200e-6 //seconds
#define TIM2_PRESCALER 64 //Dependant on the setupTIM2_ISR() function
#define TIM2_START 256-TICK_TIME*CPU_FREQ/TIM2_PRESCALER

uint16_t g_interrupt_count=0;
Flagger g_Flagger;
uint8_t g_1000ms_flag = g_Flagger.registerFlag(5000);

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

void setupTIM2_ISR(){
	//Register definitions found Page ~157 in datasheet
	//TCNT2   //Timer/Counter Register. The actual timer value is stored here.
  //TCCR2B  //Timer Control Register (Prescaler here)
	//TIMSK2  //Timer Interrupt Mask Register
 
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
  TIMSK2 |= (1 << TOIE2);
  TCNT2=TIM2_START;

}

ISR(TIMER2_OVF_vect){
  TCNT2=TIM2_START; //Reset the timer to start value for consistant roll overs
  g_interrupt_count++;
	g_Flagger.tick();
  //Flag cleared automagically
}

void loop()
{
  if (g_interrupt_count>5000){
    Serial.print("Interrupt Count=");
    Serial.println(g_interrupt_count);
    g_interrupt_count=0;
  }

	//This happens once a second
	if (g_Flagger.getFlag(g_1000ms_flag)){
		digitalWrite(ledPin, digitalRead(ledPin) ^ 1); //Toggle LED
		g_Flagger.clearFlag(g_1000ms_flag);
	}
}

