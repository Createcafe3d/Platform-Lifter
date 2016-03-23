#include "FlaggerHandler.h"
#include "PeachyTimer2Setup.h"

#ifdef STEPPER_STROBE
	extern PeachyStrobeStepper g_Stepper;
#else
	extern PeachyStepper g_Stepper;
#endif

//externals
extern PeachyFlagger g_Flagger;
extern PeachyPrintState g_PrintState;

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
	g_PrintState.initializeFlags();

}

//This happens once a second
void oneSecondHandler(){
	static uint8_t move_direction=0;
	if (g_Flagger.getFlag(g_1000ms_flag)){
		digitalWrite(LED_BLUE_PIN, move_direction); //Toggle LED
		move_direction = !move_direction;
		g_Flagger.clearFlag(g_1000ms_flag);
	}
}

//Hard stop on the way up Limit switch
void limitSwitchHandler(){
	if (g_Flagger.getFlag(g_limit_switch_flag)){
		if (!digitalRead(LIMIT_PIN)){ //active low
			digitalWrite(LED_RED_PIN,1);
			g_Stepper.stop();
			g_Stepper.move(STEPPER_DOWN,STEPPER_LIMIT_BOUNCEBACK);
			g_Stepper.limited(1);
			g_system_state=STATE_LIMITED;
		}
	}
	g_Flagger.clearFlag(g_limit_switch_flag);
}

void startButtonHandler(){
	if (digitalRead(START_BUTTON_PIN) == 0){
		Serial.write("START\n");
		g_PrintState.start(0); //Start it if it isn't already moving.
		g_PrintState.externalTrigger();
		g_drips_requested=1;
	}
}

//Both Buttons, reset and initialize analog
void buttonHandler(){
	if (g_Flagger.getFlag(g_buttons_flag)){
		//Start Button (debug trigger)
		startButtonHandler();

		//Reset Button (red)
		if (digitalRead(RESET_BUTTON_PIN) == 0){
			digitalWrite(LED_RED_PIN,0);
			digitalWrite(LED_YELLOW_PIN,0);
			g_Stepper.stop();
			g_system_state=STATE_NORMAL;
			g_Stepper.limited(0);
			findUpperLimit();
		}
		//Set Height Button (blue)
		if (g_system_state == STATE_NORMAL){
			//NORMAL
			if (digitalRead(HEIGHT_BUTTON_PIN) == 0){
				g_Stepper.stop();
				g_system_state=STATE_ANALOG;
				digitalWrite(LED_YELLOW_PIN,1);
			}
		}
		else if (g_system_state == STATE_LIMITED){
			//LIMITED case
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
	static uint8_t sent_drips=0;

	if (g_Flagger.getFlag(g_drip_flag)){
		//Send the drips to start the layer
		if ((sent_drips == 0) & (g_PrintState.getState()==PRINT_STATE_PRINTING)){
			sent_drips=1;
			g_drips_requested=2;
		}
		else if (g_PrintState.getState()!=PRINT_STATE_PRINTING){
			sent_drips=0;
		}

		//manual dripping, set drip counts
		if (g_drips_requested > 0){
			g_drips_requested=g_drips_requested-1;
			drip=true;
		}
		else{
			drip=false;
		}

		if ((drip)){
			for (uint8_t i=0; i<DRIP_TOGGLES; i++){
				digitalWrite(DRIP_PIN,1);
				delayMicroseconds(100);
				digitalWrite(DRIP_PIN,0);
				delayMicroseconds(100);
			}
		}
		g_Flagger.clearFlag(g_drip_flag);
	}
}
