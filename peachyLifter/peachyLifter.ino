#include "Arduino.h"

#include "PeachyDefines.h"

//function prototypes
void findUpperLimit();
void goToNewStartHeight();
void printSetups();

//Setup Files:
//g_Flagger and g_Stepper are found in PeachyTimer2Interrupt.h
#include "PeachyTimer2Interrupt.h"
#include "FlaggerHandler.h"
#include "serialHandler.h"

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
  printSetups();
  interrupts();
  
  Serial.begin(SERIAL_BAUD);

	//Serial.buffer doesn't seem to exist in 1.6.8 version of arduino library
	// ... thanks arduino
	//Serial.buffer(SERIAL_NUMBYTES_TRIGGER); //Buffer N bytes before calling serialEvent()

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
}

//***********************************
//*** Extra Functions ***************
//***********************************

void findUpperLimit(){
  uint8_t stepper_direction;
  
  while(digitalRead(LIMIT_PIN)){
		serialEvent();
    if (g_Stepper.getDirection() == STEPPER_STABLE){
      g_Stepper.move(STEPPER_UP);
    }
  }
    
  g_Stepper.move(STEPPER_DOWN,LIMIT_SWITCH_BUFFER_STEPS);
  g_Stepper.waitForMove();
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
  Serial.println(analog_result);
	g_resin_height = (0-(int32_t)analog_result*ANALOG_SCALER);
	g_layer_float = g_resin_height;
  g_Stepper.moveTo(g_resin_height); //0 minus so that we travel DOWN to absolute positions, relative to 0
  g_Stepper.waitForMove();
	delay(1);
}

