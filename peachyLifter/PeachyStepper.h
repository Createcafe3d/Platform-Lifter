// This is Peachys Stepper motor driver
// It uses a half-stepping for 4 coils
#ifndef _PEACHYSTEPPER_H_
#define _PEACHYSTEPPER_H_

#define STEPPER_PIN0 8
#define STEPPER_PIN1 9
#define STEPPER_PIN2 10
#define STEPPER_PIN3 11

#define STEPPER_STABLE 2
#define STEPPER_UP 1
#define STEPPER_DOWN 0

#define MASK_TOPBIT 0b10000000
#define MASK_BOTBIT 0b00000001

#define MASK_PIN0 0b00000010
#define MASK_PIN1 0b00001000
#define MASK_PIN2 0b00100000
#define MASK_PIN3 0b10000000

#include <stdint.h>
class PeachyStepper
{
	public:
		PeachyStepper(){
			m_current_position=0;
			m_commanded_position=0;
			m_direction = STEPPER_UP;
			m_hold_torque = 0; 
			m_step_state=7;//[00000111]
		}

		void move(uint8_t direction,uint32_t steps){
			//direction 0 or 1 depending on the wiring
			if (direction == STEPPER_DOWN)
				m_commanded_position-=steps;
			else if (direction == STEPPER_UP)
				m_commanded_position+=steps;
		}

		void move (uint8_t direction){
			//direction 0 or 1 depending on the wiring
			if (direction == STEPPER_DOWN)
				m_commanded_position--;
			else if (direction == STEPPER_UP)
				m_commanded_position++;
		}

		void step(){
			//Function called by the timer interrupt 
			if (m_current_position == m_commanded_position){
				//Switch to holding torque mode
				//which for now means doing nothing
				m_direction=STEPPER_STABLE
			}
			else{
				if (m_current_position > m_commanded_position)
					m_direction = STEPPER_UP;
					m_current_position++;
				else // <
					m_direction = STEPPER_DOWN;
					m_current_position--;
				shift_step();
				assign_bits();
			}

		void zero_position(direction){


		}

	private:
		uint8_t m_direction;
		uint8_t m_hold_torque;
		uint8_t m_step_state;
		int32_t m_commanded_position;
		int32_t m_current_position;

		void shift_step(){
			uint8_t carry_bit;

			if (m_direction == STEPPER_UP){
				carry_bit=m_step_state && MASK_TOPBIT;
				m_step_state=m_step_state<<1; //Shift up
				if (carry_bit)
					m_step_state+=0x01;//Bring in the carry
			}
			else if(m_direction == STEPPER_DOWN){
				carry_bit=m_step_state && MASK_BOTBIT;
				m_step_state=m_step_state>>1; //Shift down
				if (carry_bit)
					m_step_state+=0x80;//Add in the carry at the top
			}
		}

		void assign_bits()
		{
			// 4 pin servos "on" steps:
			// [1 0 0 0] 0 
			// [1 1 0 0] 1 
			// [0 1 0 0] 2 
			// [0 1 1 0] 3 
			// [0 0 1 0] 4 
			// [0 0 1 1] 5 
			// [0 0 0 1] 6 
			// [1 0 0 1] 7 
			uint8_t tmp_bit;

			tmp_bit = |(m_step_state && MASK_PIN0);
			digitalWrite(STEPPER_PIN0,tmp_bit);

			tmp_bit = |(m_step_state && MASK_PIN1);
			digitalWrite(STEPPER_PIN1,tmp_bit);
			
			tmp_bit = |(m_step_state && MASK_PIN2);
			digitalWrite(STEPPER_PIN2,tmp_bit);

			tmp_bit = |(m_step_state && MASK_PIN3);
			digitalWrite(STEPPER_PIN3,tmp_bit);
		}

#endif
