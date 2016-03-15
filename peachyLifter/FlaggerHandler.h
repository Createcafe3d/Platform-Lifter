#ifndef _FLAGGER_HANDLER
#define _FLAGGER_HANDLER

#include "PeachyDefines.h"

#define STEPPER_LIMIT_BOUNCEBACK 20

//Quick Examples for steps -> milliseconds
//TICK_TIME is defined in the PeachyDefines.h
#define TICK_1000MS 1/TICK_TIME
#define TICK_100MS	0.1/TICK_TIME
#define TICK_10MS		0.01/TICK_TIME
#define TICK_1MS		0.001/TICK_TIME

#define DRIP_TIME					 (uint16_t)(2 * TICK_100MS) //100ms minimum dead time between drips by design
#define BUTTON_TIME				 (uint16_t)(4 * TICK_10MS)
#define ANALOG_TIME				 (uint16_t)(6 * TICK_10MS)
#define LIMIT_SWITCH_TIME  (uint16_t)(5 * TICK_10MS)

uint8_t g_1000ms_flag				= g_Flagger.registerFlag(TICK_1000MS);
uint8_t g_drip_flag					= g_Flagger.registerFlag(DRIP_TIME);
uint8_t g_buttons_flag			= g_Flagger.registerFlag(BUTTON_TIME);
uint8_t g_analog_flag				= g_Flagger.registerFlag(ANALOG_TIME);
uint8_t g_limit_switch_flag = g_Flagger.registerFlag(LIMIT_SWITCH_TIME);

//globals
uint8_t g_drips_requested=0;
uint8_t g_system_state=STATE_NORMAL;
int32_t g_resin_height=0;

//This happens once a second
void oneSecondHandler(){
	uint8_t move_direction;
	if (g_Flagger.getFlag(g_1000ms_flag)){
		g_Flagger.clearFlag(g_1000ms_flag);

		Serial.println("ONE SECOND");
		move_direction=digitalRead(LED_BLUE_PIN);
		//g_Stepper.move(move_direction,500);
		digitalWrite(LED_BLUE_PIN, move_direction ^ 1); //Toggle LED
		//g_drips_requested = 1;
	}
}

//Hard stop on the way up Limit switch
void limitSwitchHandler(){
	if (g_Flagger.getFlag(g_limit_switch_flag)){
		if (!digitalRead(LIMIT_PIN)){ //active low
			digitalWrite(LED_RED_PIN,1);
			g_Stepper.stop();
			g_Stepper.move(STEPPER_DOWN,STEPPER_LIMIT_BOUNCEBACK);
			g_system_state=STATE_LIMITED;
			if (digitalRead(RESET_BUTTON_PIN) == 0){
				g_Stepper.stop();
				findUpperLimit();
			}
			g_Flagger.clearFlag(g_limit_switch_flag);
		}
	}
}

//Both Buttons, reset and initialize analog
void buttonHandler(){
	if (g_Flagger.getFlag(g_buttons_flag)){
		if (g_system_state == STATE_NORMAL){
			if (digitalRead(RESET_BUTTON_PIN) == 0){
				g_Stepper.stop();
				findUpperLimit();
			}
			if (digitalRead(HEIGHT_BUTTON_PIN) == 0){
				g_Stepper.stop();
				g_system_state=STATE_ANALOG;
			}
		}
		else if (g_system_state == STATE_LIMITED){
			if (digitalRead(RESET_BUTTON_PIN) == 0){
				findUpperLimit();
				digitalWrite(LED_RED_PIN,0);
				g_system_state = STATE_NORMAL;
			}
		}
		g_Flagger.clearFlag(g_buttons_flag);
	}
}

void dripHandler(){
	uint8_t drip = false;

	if (g_Flagger.getFlag(g_drip_flag)){
		//manual dripping, set drip counts
		if (g_drips_requested > 0){
			g_drips_requested--;
			drip=true;
			}
		if ((drip) | (g_dripper_state == ON)){
			for (uint8_t i=0; i<DRIP_TOGGLES; i++){
				digitalWrite(DRIP_PIN,1);
				delayMicroseconds(100);
				digitalWrite(DRIP_PIN,0);
				delayMicroseconds(100);
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

#endif

