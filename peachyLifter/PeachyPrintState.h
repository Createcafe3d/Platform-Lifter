#ifndef _PEACHY_PRINTSTATE
#define _PEACHY_PRINTSTATE


#define PRINT_STATE_PRINTING 0
#define PRINT_STATE_RESURRECTING 1
#define PRINT_STATE_SUBMERGING 2
#define PRINT_STATE_LIFTING 3
#define PRINT_STATE_FLOWING 4

#define NUMBER_PRINT_STATES 5

#define CAMERA_PIN A5

#define PICTURE_QUICK_TIME  0.2 //seconds
#define PICTURE_QUICK_TICKS PICTURE_QUICK_TIME/TICK_TIME

#define PICTURE_LONG_TIME  10 //seconds
#define PICTURE_LONG_TICKS PICTURE_LONG_TIME/TICK_TIME

#include <stdint.h>
#include "PeachyDefines.h"
#include "PeachyTimer2Setup.h"
#include "FlaggerHandler.h" //for the defines

extern PeachyFlagger g_Flagger;
extern PeachyStepper g_Stepper;

struct PeachyStates{
	uint16_t delay_ticks = 0;		//ticks delay before layer drawing
	int32_t absoluteHeight_steps = 0;			//Absolute print surface height
	uint8_t photoBeforeDelay = false;
	uint8_t photoAfterDelay = false;
	uint8_t photoDuringDelay = false;
};

class PeachyPrintState
{
	public:
		PeachyPrintState(){
			m_layerHeight_mm = MICROMETERS_PER_LAYER*1e3; //for now
			m_layerHeight_steps = (uint16_t)((m_layerHeight_mm*1000)/MICROMETERS_PER_STEP);
			m_flagger_id_state = g_Flagger.registerFlag(0); //disabled for now
			m_flagger_id_picture = g_Flagger.registerFlag(0); //disabled for now
			m_resin_height_steps = 0;
			m_printState = PRINT_STATE_PRINTING;
		}

		//Seconds and Millimeters
		void initializeStateDistanceTime(uint8_t state, float delay=0.0, float height_from_resin=0, uint8_t photoBeforeDelay=false, uint8_t photoAfterDelay=false){
			uint16_t tick_delay = delay/TICK_TIME;
			int32_t step_height = (height_from_resin/MILLIMETERS_PER_STEP)+m_resin_height_steps;
			initializeState(state, tick_delay, step_height, photoBeforeDelay, photoAfterDelay);
		}

		//Ticks and Steps
		void initializeState(uint8_t state, uint16_t delay=0, int32_t height=0,uint8_t photoDuringDelay=false, uint8_t photoBeforeDelay=false, uint8_t photoAfterDelay=false){
			m_printStates[state].delay_ticks=delay;
			m_printStates[state].absoluteHeight_steps=height;
			m_printStates[state].photoBeforeDelay=photoBeforeDelay;
			m_printStates[state].photoAfterDelay=photoAfterDelay;
			m_printStates[state].photoDuringDelay=photoDuringDelay;
		}

		void takePicture(){
			//quick snap
			g_Flagger.updateTrigCount(m_flagger_id_picture, PICTURE_QUICK_TICKS); //enable it and clear it 
			digitalWrite(CAMERA_PIN,1);
		}

		void takeDuringPicture(){
			//Keep it on for a longer delay
			g_Flagger.updateTrigCount(m_flagger_id_picture, PICTURE_LONG_TICKS); //enable it and clear it 
			digitalWrite(CAMERA_PIN,1);
		}

		void pictureHandler(){
			//Figure out when to turn it off
			if (g_Flagger.getFlag(m_flagger_id_picture)){
				digitalWrite(CAMERA_PIN,0);
				g_Flagger.disable(m_flagger_id_picture);
				g_Flagger.clearFlag(m_flagger_id_picture);
			}
		}

		void handlePrintState(){
			pictureHandler(); //Handle the camera triggering OFF

			if (g_Flagger.getFlag(m_flagger_id_state)){
				//Just finished the delay cycle
				if (m_printStates[m_printState].photoAfterDelay){
					takePicture();
				}
				printStateMachineNext();//Now that we are done and took a picture (maybe), go to next state
				m_delay=true;//Do the delay action on next handle call
				g_Flagger.clearFlag(m_flagger_id_state);
			}

			//ready to start a new delay cycle, do we need to start a new delay?
			else {
				if (m_printState == PRINT_STATE_PRINTING){
					//special case to wait for the layer to be done
					uint8_t i=0;
				}
				else if (m_delay & (g_Stepper.getDirection() == STEPPER_STABLE)){
					if (m_printStates[m_printState].photoBeforeDelay){
						takePicture();
					}
					if (m_printStates[m_printState].photoDuringDelay){
						takeDuringPicture();
					}
					g_Flagger.updateTrigCount(m_flagger_id_state, m_printStates[m_printState].delay_ticks);
					m_delay=false;
				}
			}
		}

		//Steps to print:
		//1) DRAWING: draw a layer
		//2) Raise print all the way up - Pictures
		//3) Totally submerge print - Pictures
		//4) Bring print to printing height + Margin1 (This is to drain off resin that is globbed on top) - Pictures
		//5) Bring print to printing height - Margin2 (This is to allow the surface tension to flow back over) - Pictures
		//6) Bring print to printing height (This is to allow the surface tension to flow back over) - Pictures (timelapse mode optional)
		//
		//Print -> Resurrect -> Submerge -> Lift -> Flow -> Print

	private:

		void printStateMachineNext(){
			//call when done current state
			switch(m_printState){
				case PRINT_STATE_PRINTING:
					m_printState=PRINT_STATE_RESURRECTING;
					break;
				case PRINT_STATE_RESURRECTING:
					m_printState=PRINT_STATE_SUBMERGING;
					break;
				case PRINT_STATE_SUBMERGING:
					m_printState=PRINT_STATE_LIFTING;
					break;
				case PRINT_STATE_LIFTING:
					m_printState=PRINT_STATE_FLOWING;
					break;
				case PRINT_STATE_FLOWING:
					m_printState=PRINT_STATE_PRINTING;
					break;
			}
			g_Stepper.moveTo(m_printStates[m_printState].absoluteHeight_steps);//Start the move to that height
		}

		uint8_t m_flagger_id_state;
		uint8_t m_flagger_id_picture;
		uint8_t m_printState;
		uint8_t m_delay=false;
		int32_t m_resin_height_steps;

		float m_layerHeight_mm;       //mm per layer
		uint16_t m_layerHeight_steps;

		PeachyStates m_printStates[NUMBER_PRINT_STATES];
};
#endif
