// This is Peachys Stepper motor driver
// It uses a half-stepping for 4 coils
#ifndef _PEACHYSTEPPER_H_
#define _PEACHYSTEPPER_H_

#include "PeachyDefines.h"
#include <stdint.h>

#ifdef _TEST_PEACHYSTEPPER
	void digitalWrite(unsigned char,unsigned char); //Stub it out for testing
	void delayMicroseconds(int); //Stub out arduino delay
#else
	#include "Arduino.h"
#endif

#define MASK_TOPBIT 0b10000000
#define MASK_BOTBIT 0b00000001

#define MASK_PIN0 0b00000010
#define MASK_PIN1 0b00001000
#define MASK_PIN2 0b00100000
#define MASK_PIN3 0b10000000

class PeachyStepper
{
	public:
		PeachyStepper(uint8_t hold_torque=0);

		void setSpeed(uint8_t speed) { m_speed=speed; }
		void stop() { m_commanded_position=m_current_position; }
		void limited(uint8_t state);
		void moveTo(int32_t position);
		void zeroPosition(){
			m_current_position = 0;
			m_commanded_position = 0;
		}

		void move(uint8_t direction,uint32_t steps);
		void move(uint8_t direction);
    void waitForMove();

		virtual void microStep();
		virtual void step();


		int32_t getPosition(){
			return m_current_position;
		}

		int32_t getCommandedPosition(){
			return m_commanded_position;
		}

		uint8_t getState(){
			return m_step_state;
		}

		uint8_t getDirection(){
			setDirection();
			return m_direction;
		}

	protected:
		uint8_t m_direction;
		uint8_t m_limited;
		int32_t m_commanded_position;
		int32_t m_current_position;
    void setDirection();

	private:
		uint8_t m_hold_torque;
		uint8_t m_step_state;
		uint8_t m_microstep_counter;
		uint8_t m_speed;
		uint8_t m_speed_counter;
		uint8_t m_force_pins;

		void shift_step();
		void pins_off();
		void pins_on();
		void assign_bits();
};
#endif
