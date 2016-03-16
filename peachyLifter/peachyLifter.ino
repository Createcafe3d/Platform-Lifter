#include "Arduino.h"

#include "PeachyDefines.h"
#include "PeachyMain.h"

//g_Flagger and g_Stepper are found in PeachyTimer2Setup.h
#include "PeachyTimer2Setup.h"
#include "SerialHandler.h"
#include "FlaggerHandler.h"
#include "PeachyPrintState.h"
#include <stdint.h>

//externs
extern PeachyFlagger g_Flagger;
extern PeachyStepper g_Stepper;
extern uint16_t g_interrupt_count;
extern uint16_t g_Serial_starved_count;
extern uint8_t g_Serial_starved;
extern int32_t g_resin_height;
extern double g_layer_float;

PeachyPrintState g_PrintState();

void setup()
{
	pinMode(LIMIT_PIN,INPUT_PULLUP); 
	pinMode(LED_RED_PIN,OUTPUT); 
	pinMode(LED_BLUE_PIN,OUTPUT); 
  pinMode(DRIP_PIN,OUTPUT);
  pinMode(RESET_BUTTON_PIN,INPUT_PULLUP);
  pinMode(HEIGHT_BUTTON_PIN,INPUT_PULLUP);

	noInterrupts();
	setupTIM2_ISR();
  interrupts();

	initialize_flags();
  printSetups();
  
  Serial.begin(SERIAL_BAUD);

  findUpperLimit();
	g_Stepper.setSpeed(1); //1/X speed, where X is the argument
}

void loop()
{
  uint16_t tmp_count;
  uint8_t stepper_direction;
  
	//debug function:
  if (g_interrupt_count>5000){
    tmp_count=g_interrupt_count;
    Serial.print("Interrupt Count=");
    Serial.println(tmp_count);
    g_interrupt_count=0;
  }

	//Handler functions found in FlaggerHandler.h
	limitSwitchHandler();
  dripHandler();
  buttonHandler();
  analogHeightHandler();
	oneSecondHandler();

	//Handle the print state machine
	g_PrintState.handlePrintState();

}

//***********************************
//*** Extra Functions ***************
//***********************************

//This function gets called every TICK_TIME
ISR(TIMER2_OVF_vect){
  TCNT2=TIM2_START; //Reset the timer to start value for consistant roll overs
  g_interrupt_count++;//my favourite debug counter
	g_Serial_starved_count++;
	g_Flagger.tick();
  g_Stepper.microStep();
	serialCheckStarved();
  //Interrupt Flag cleared automagically
}

void findUpperLimit(){
  uint8_t stepper_direction;

  while(digitalRead(LIMIT_PIN)){
		serialEvent();
    if (g_Stepper.getDirection() == STEPPER_STABLE){
      g_Stepper.move(STEPPER_UP);
    }
  }
	Serial.write("AFTER serialEvent\n");
  g_Stepper.move(STEPPER_DOWN,LIMIT_SWITCH_BUFFER_STEPS);
	Serial.write("AFTER serialEvent move \n");
  g_Stepper.waitForMove();
	Serial.write("AFTER serialEvent waitForMove\n");
  g_Stepper.zeroPosition();
  goToNewStartHeight();
}

void printSetups(){
  Serial.print("tim2_start is:");
  Serial.println(TIM2_START);
  Serial.print("prescaler is:");
  Serial.println(TIM2_PRESCALER);
}

void goToNewStartHeight()
{
  uint16_t analog_result;
  analog_result = analogRead(HEIGHT_ANALOG_PIN);
	g_resin_height = (0-(int32_t)analog_result*ANALOG_SCALER);
	g_layer_float = g_resin_height;
  g_Stepper.moveTo(g_resin_height); //0 minus so that we travel DOWN to absolute positions, relative to 0
}

