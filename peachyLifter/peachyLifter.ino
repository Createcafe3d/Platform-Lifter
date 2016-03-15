#include "Arduino.h"

//g_Flagger and g_Stepper are found in PeachyTimer2Interrupt.h
#include "PeachyTimer2Interrupt.h"

#define LED_PIN 7
#define LIMIT_PIN 12
#define DRIP_PIN A4
#define DRIP_TOGGLES 8 //Number of half cycles (on-off is 2 half cycles)
#define RESET_BUTTON_PIN 11
#define HEIGHT_BUTTON_PIN 10
#define HEIGHT_ANALOG_PIN A3
#define LIMIT_SWITCH_BUFFER_STEPS 500

#define ANALOG_SCALER 3 //How much to multiply up the Analog reading by. Analog values range from 0->1023
#define STATE_ANALOG 1
#define STATE_NORMAL 0

//Flag Tick time == 200us
// 5000 == 1 second
// 500 == 100ms

uint8_t g_1000ms_flag = g_Flagger.registerFlag(5000);
uint8_t g_drip_flag = g_Flagger.registerFlag(1000);
uint8_t g_buttons_flag = g_Flagger.registerFlag(300);
uint8_t g_analog_flag = g_Flagger.registerFlag(2000);

uint8_t g_drips_requested=0;
uint8_t g_system_state=STATE_NORMAL;

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

void buttonHandler(){
  if (g_Flagger.getFlag(g_buttons_flag)){
    if (digitalRead(RESET_BUTTON_PIN) == 0){
      g_Stepper.stop();
      findUpperLimit();
    }
    if (digitalRead(HEIGHT_BUTTON_PIN) == 0){
    //while(digitalRead(HEIGHT_BUTTON_PIN == 0)){
      g_Stepper.stop();
      g_system_state=STATE_ANALOG;
    }
    g_Flagger.clearFlag(g_buttons_flag);
  }
}

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

void dripHandler(){

  if (g_Flagger.getFlag(g_drip_flag)){
    if (g_drips_requested > 0){
      g_drips_requested--;
      for (uint8_t i=0; i<DRIP_TOGGLES; i++){
        digitalWrite(DRIP_PIN,1);
        delay(1);
        digitalWrite(DRIP_PIN,0);
      }
      g_Flagger.clearFlag(g_drip_flag);
    }
  }
}

void analogHeightHandler(){
  if (g_system_state==STATE_ANALOG){
    findUpperLimit(); //blocking
    while(g_system_state==STATE_ANALOG){
      goToNewStartHeight();
      while (digitalRead(HEIGHT_BUTTON_PIN)==0){
        g_system_state=STATE_NORMAL;
        findUpperLimit();
      }
    }
  }
}

void goToNewStartHeight()
{
  uint16_t analog_result;
  analog_result = analogRead(HEIGHT_ANALOG_PIN);
  Serial.println(analog_result);
  g_Stepper.moveTo(0-(int16_t)analog_result*ANALOG_SCALER); //0 minus so that we travel DOWN to absolute positions, relative to 0
  g_Stepper.waitForMove();
}
  





void loop()
{
  uint8_t move_direction;
  uint16_t tmp_count;
  uint8_t stepper_direction;
  
  if (g_interrupt_count>5000){
    tmp_count=g_interrupt_count;
    Serial.print("Interrupt Count=");
    Serial.println(tmp_count);
    g_interrupt_count=0;
  }

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

