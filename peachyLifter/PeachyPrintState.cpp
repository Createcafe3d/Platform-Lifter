#include "PeachyPrintState.h"

#ifdef STEPPER_STROBE
	extern PeachyStrobeStepper g_Stepper;
#else
	extern PeachyStepper g_Stepper;
#endif

extern PeachyFlagger g_Flagger;

PeachyPrintState::PeachyPrintState(){
	m_layerHeight_mm = MICROMETERS_PER_LAYER*1e3; //for now
	m_layerHeight_steps = (uint16_t)((m_layerHeight_mm*1000)/MICROMETERS_PER_STEP);
	m_resin_height_steps = 0;
	m_printState = PRINT_STATE_PRINTING;
}

void PeachyPrintState::setResinHeight(int32_t height){
	m_resin_height_steps=height;
}

void PeachyPrintState::start(){
	g_Flagger.enable(m_flagger_id_state);
}

void PeachyPrintState::stop(){
	g_Flagger.disable(m_flagger_id_state);
}

void PeachyPrintState::start(uint8_t startState){
	m_printState=startState;
	g_Flagger.updateTrigCount(m_flagger_id_state, m_printStates[m_printState].delay_ticks);
}

void PeachyPrintState::initializeFlags(){
	m_flagger_id_state = g_Flagger.registerFlag(0); //disabled for now
	m_flagger_id_picture = g_Flagger.registerFlag(0); //disabled for now
}

void PeachyPrintState::initializeStateDistanceTime(uint8_t state, float delay, float height_from_resin, uint8_t photoDuringDelay, uint8_t photoBeforeDelay, uint8_t photoAfterDelay, uint8_t externalTrigger){
			uint16_t tick_delay = delay/TICK_TIME;
			int32_t step_height = (height_from_resin/MILLIMETERS_PER_STEP)+m_resin_height_steps;
			Serial.write("Tick Delay: ");
			Serial.println(tick_delay);
			Serial.write("resin_height_steps : ");
			Serial.println(m_resin_height_steps);
			Serial.write("step_height: ");
			Serial.println(step_height);
			initializeState(state, tick_delay, step_height, photoDuringDelay, photoBeforeDelay, photoAfterDelay, externalTrigger);
}

void PeachyPrintState::printStates(){
	Serial.write("printing ALL printStates\n");
	for (uint8_t i=0;i<NUMBER_PRINT_STATES;i++){
		Serial.println(m_printStates[i].delay_ticks);
		Serial.println(m_printStates[i].absoluteHeight_steps);
	}
}

void PeachyPrintState::initializeState(uint8_t state, uint16_t delay, int32_t height ,uint8_t photoDuringDelay, uint8_t photoBeforeDelay, uint8_t photoAfterDelay, uint8_t externalTrigger){
	m_printStates[state].delay_ticks=delay;
	m_printStates[state].absoluteHeight_steps=height;
	m_printStates[state].photoBeforeDelay=photoBeforeDelay;
	m_printStates[state].photoAfterDelay=photoAfterDelay;
	m_printStates[state].photoDuringDelay=photoDuringDelay;
	m_printStates[state].externalTrigger=externalTrigger;
}

void PeachyPrintState::takePicture(){
	//quick snap
	g_Flagger.updateTrigCount(m_flagger_id_picture, PICTURE_QUICK_TICKS); //enable it and clear it 
	m_picture_pin_state=1;
	digitalWrite(CAMERA_PIN,1);
}

void PeachyPrintState::takeDuringPicture(){
	//Keep it on for a longer delay
	g_Flagger.updateTrigCount(m_flagger_id_picture, PICTURE_LONG_TICKS); //enable it and clear it 
	m_picture_pin_state=1;
	digitalWrite(CAMERA_PIN,1);
}

void PeachyPrintState::externalTrigger(uint8_t state){
	m_printStates[state].externalTriggered=1;
}

void PeachyPrintState::updateHeightSteps(uint8_t state,int32_t steps){
	m_printStates[state].absoluteHeight_steps=steps;
}

uint8_t PeachyPrintState::getState(){
	return m_printState;	
}


void PeachyPrintState::handlePrintStates(){
	pictureHandler();
	handleStartPrintState();
	handleFinishedPrintState();
}

void PeachyPrintState::pictureHandler(){
	//Figure out when to turn it off
	if (g_Flagger.getFlag(m_flagger_id_picture)){
		Serial.write("pic\n");
		digitalWrite(CAMERA_PIN,0);
		m_picture_pin_state=0;
		g_Flagger.disable(m_flagger_id_picture);
		g_Flagger.clearFlag(m_flagger_id_picture);
	}
}

void PeachyPrintState::handleFinishedPrintState(){
	static uint8_t taken_after_picture=0;

	//check for: Ready for next state, AND we aren't currently taking a picture
	if (m_picture_pin_state == 0){
		if (g_Flagger.getFlag(m_flagger_id_state)){
			if ((m_printStates[m_printState].photoAfterDelay) & (taken_after_picture == 0)){
				taken_after_picture=1;
				takePicture(); //sets picture_pin to 1 until picture taking is done
			}
			else{
				taken_after_picture=0;
				m_finished_state=true;//Done!
				g_Flagger.clearFlag(m_flagger_id_state);
				g_Flagger.disable(m_flagger_id_state);
			}
		}
	}
}

void PeachyPrintState::handleStartPrintState(){
	static uint8_t taken_during_picture=0;
	static uint8_t taken_before_picture=0;
	static uint8_t update_trig_count=0;
	
	//Check that we still aren't taking a picture
	if (m_picture_pin_state == 0){

		//Finish the current State (are we finishing this state?)
		if (m_finished_state) {
			//This should be a NAND... deMorgan another day
			if (m_printStates[m_printState].externalTrigger == true ){
				if (m_printStates[m_printState].externalTriggered){
					Serial.write("got external Trigger!\n");
					m_finished_state=false;
					m_printStates[m_printState].externalTriggered=0;
					update_trig_count=1;
					taken_during_picture=0;
					taken_before_picture=0;
					m_printState = (m_printState+1)%NUMBER_PRINT_STATES; //Move to next stage
					g_Stepper.moveTo(m_printStates[m_printState].absoluteHeight_steps);
				}
			}
			else{ //else I'm done move on and reset states
				m_finished_state=false;
				update_trig_count=1; //update to the next delay
				taken_during_picture=0;
				taken_before_picture=0;
				m_printState = (m_printState+1)%NUMBER_PRINT_STATES; //Move to next stage
				g_Stepper.moveTo(m_printStates[m_printState].absoluteHeight_steps); //go there
			}
		}

		//Else kick off the next state (when we are done moving to it)
		else if (g_Stepper.getDirection() == STEPPER_STABLE)  {
			if (update_trig_count == 1){
				update_trig_count = 0;
				g_Flagger.updateTrigCount(m_flagger_id_state, m_printStates[m_printState].delay_ticks);
			}
			else if ((m_printStates[m_printState].photoBeforeDelay) & (taken_before_picture == 0)){
				taken_before_picture=1;
				takePicture();
			}
			else if ((m_printStates[m_printState].photoDuringDelay) & (taken_during_picture == 0)){
				taken_during_picture=1;
				takeDuringPicture();
			}
		}
	}
}
