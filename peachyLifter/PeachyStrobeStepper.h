#ifndef _PEACHYSTROBESTEPPER_H_
#define _PEACHYSTROBESTEPPER_H_

#include "Arduino.h"
#include "PeachyDefines.h"
//From PeachyDefines, I'm using pins 0-2 as Enable,Direction,Step respectively

#define STEPPER_EN_PIN				2
#define STEPPER_DIRECTION_PIN 3
#define STEPPER_STROBE_PIN		4

#define DOWN	0
#define UP	  1	

class PeachyStrobeStepper: public PeachyStepper{

	public:
		PeachyStrobeStepper(uint8_t hold_torque):PeachyStepper(hold_torque){}

		virtual void microStep(){
			//Serial.write("I MICROSTEPPED!\n");
			//Serial.println(m_current_position);
			//Serial.println(m_commanded_position);
			step();
		} //no microstepping in this one

		virtual void step(){
			Serial.write("STEP\n");
			static uint8_t stepState=1;
			setDirection();

			if (false)
				digitalWrite(STEPPER_EN_PIN,1); //If not limited, then enabled

			else{ //On even steps, calculate direction. On odd steps update position
				digitalWrite(STEPPER_EN_PIN,0); //If not limited, then enabled
				if (stepState){
					Serial.write("STROBE 1\n");
					digitalWrite(STEPPER_STROBE_PIN,1);
					if (m_direction == STEPPER_DOWN)
						m_current_position--;
					else if (m_direction == STEPPER_UP)
						m_current_position++;
					stepState=0;
				}
				else{
					Serial.write("STROBE 0\n");
					digitalWrite(STEPPER_STROBE_PIN,0);
					if (m_direction == STEPPER_DOWN)
						digitalWrite(STEPPER_DIRECTION_PIN, DOWN);
					else
						digitalWrite(STEPPER_DIRECTION_PIN, UP);
					stepState=1;
				}
			}
		}
};

#endif
