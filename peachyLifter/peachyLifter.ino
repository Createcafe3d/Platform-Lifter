#include "Arduino.h"

#include "PeachyDefines.h"

//globals
uint8_t g_drips_requested=0;
uint8_t g_system_state=STATE_NORMAL;

//function prototypes (for now)
void findUpperLimit();
void goToNewStartHeight();
void printSetups();

//Setup Files:
//g_Flagger and g_Stepper are found in PeachyTimer2Interrupt.h
#include "PeachyTimer2Interrupt.h"
#include "FlaggerHandler.h"

void setup()
{
	pinMode(LIMIT_PIN,INPUT_PULLUP); 
	pinMode(LED_PIN,OUTPUT); 
  pinMode(DRIP_PIN,OUTPUT);
  pinMode(RESET_BUTTON_PIN,INPUT_PULLUP);
  pinMode(HEIGHT_BUTTON_PIN,INPUT_PULLUP);

	noInterrupts();
	setupTIM2_ISR();
  printSetups();
  interrupts();
  
  Serial.begin(115200);
  findUpperLimit();
}

void loop()
{
  uint8_t move_direction;
  uint16_t tmp_count;
  uint8_t stepper_direction;
  
	g_Stepper.setSpeed(1);
  if (g_interrupt_count>5000){
    tmp_count=g_interrupt_count;
    Serial.print("Interrupt Count=");
    Serial.println(tmp_count);
    g_interrupt_count=0;
  }

	//Handler functions found in FlaggerHandler.h
  dripHandler();
  buttonHandler();
  analogHeightHandler();

//	//This happens once a second
//	if (g_Flagger.getFlag(g_1000ms_flag)){
//    Serial.println("ONE SECOND");
//    g_Flagger.clearFlag(g_1000ms_flag);
//    move_direction=digitalRead(LED_PIN);
//    g_Stepper.move(move_direction,100);
//		digitalWrite(LED_PIN, move_direction ^ 1); //Toggle LED
//    g_drips_requested = 3;
//	}
 
}

//***********************************
//*** Extra Functions ***************
//***********************************

void findUpperLimit(){
  uint8_t stepper_direction;
  
  while(digitalRead(LIMIT_PIN)){
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
  g_Stepper.moveTo(0-(int16_t)analog_result*ANALOG_SCALER); //0 minus so that we travel DOWN to absolute positions, relative to 0
  g_Stepper.waitForMove();
	delay(100);
}

