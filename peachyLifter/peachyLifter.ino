#include "Arduino.h"

void setup()
{
	Serial.begin(115200);
	pinMode(12,INPUT_PULLUP); 
  interrupts();
  
		
}
ISR(TIMER2_OVF_vect){
  int foo = 0;
}

void loop()
{
  int bar = 0;

}

