#include "FlaggerHandler.h"
#include "PeachyTimer2Setup.h"

//externals
extern PeachyFlagger g_Flagger;
extern PeachyStepper g_Stepper;

extern uint8_t g_dripper_state;

//Globals
uint8_t g_drips_requested=0;
uint8_t g_system_state=STATE_NORMAL;
int32_t g_resin_height=0;

//Global Flags
uint8_t g_1000ms_flag;
uint8_t g_drip_flag;
uint8_t g_buttons_flag;
uint8_t g_analog_flag;
uint8_t g_limit_switch_flag;

void initialize_flags(){
	g_1000ms_flag				= g_Flagger.registerFlag(TICK_1000MS);
	g_drip_flag					= g_Flagger.registerFlag(DRIP_TIME);
	g_buttons_flag			= g_Flagger.registerFlag(BUTTON_TIME);
	g_analog_flag				= g_Flagger.registerFlag(ANALOG_TIME);
	g_limit_switch_flag = g_Flagger.registerFlag(LIMIT_SWITCH_TIME);
}

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
		}
	}
	g_Flagger.clearFlag(g_limit_switch_flag);
}

//Both Buttons, reset and initialize analog
void buttonHandler(){
	if (g_Flagger.getFlag(g_buttons_flag)){
		if (digitalRead(RESET_BUTTON_PIN) == 0){
			digitalWrite(LED_RED_PIN,0);
			digitalWrite(LED_YELLOW_PIN,0);
			g_Stepper.stop();
			g_system_state=STATE_NORMAL;
			findUpperLimit();
		}
		if (g_system_state == STATE_NORMAL){
			//NORMAL
			if (digitalRead(HEIGHT_BUTTON_PIN) == 0){
				g_Stepper.stop();
				g_system_state=STATE_ANALOG;
				digitalWrite(LED_YELLOW_PIN,1);
			}
		}
		else if (g_system_state == STATE_LIMITED){
			//LIMITED
			if (digitalRead(RESET_BUTTON_PIN) == 0){
				findUpperLimit();
				digitalWrite(LED_RED_PIN,0);
				g_system_state = STATE_NORMAL;
			}
		}
	}
	g_Flagger.clearFlag(g_buttons_flag);
}

void analogHeightHandler(){
	//ANALOG
	if (g_system_state==STATE_ANALOG){
		if (digitalRead(HEIGHT_BUTTON_PIN)==0){ //if pressed, set new height
			goToNewStartHeight();
		}
		else{ //else jump out of ANALOG STATE
			g_system_state=STATE_NORMAL;
			digitalWrite(LED_YELLOW_PIN,0);
		}
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


