#include <stdint.h>
#include "PeachyStepper.h"

PeachyStepper::PeachyStepper(uint8_t hold_torque){
	m_current_position=0;
	m_commanded_position=0;
	m_direction = STEPPER_STABLE;
	m_hold_torque = hold_torque; 
	m_step_state = 7;//[00000111]
	m_microstep_counter = 0;
	m_speed=1; //Max Speed 2->half 3->third
	m_speed_counter=0;
	m_limited=0;
}

void PeachyStepper::move(uint8_t direction,uint32_t steps){
	//direction 0 or 1 depending on the wiring
	if (m_limited==0){
		if (direction == STEPPER_DOWN)
			m_commanded_position-=steps;
		else if (direction == STEPPER_UP)
			m_commanded_position+=steps;
		setDirection();
	}
}

void PeachyStepper::move(uint8_t direction){
	//direction 0 or 1 depending on the wiring
	if (m_limited==0){
		if (direction == STEPPER_DOWN)
			m_commanded_position--;
		else if (direction == STEPPER_UP)
			m_commanded_position++;
		setDirection();
	}
}

void PeachyStepper::moveTo(int32_t position){ 
	if (m_limited==0){
		m_commanded_position=position;
		setDirection();
	}
}

void PeachyStepper::waitForMove(){
	uint8_t stepper_direction;
	stepper_direction = getDirection();
	while(stepper_direction != STEPPER_STABLE){
		stepper_direction = getDirection();
	} 
}

void PeachyStepper::limited(uint8_t state){
	m_limited=state;
}

void PeachyStepper::step(){
	if (m_current_position == m_commanded_position){
		m_direction=STEPPER_STABLE;
	}
	else {
		m_speed_counter++; //never zero by design
		if (m_speed_counter>=m_speed){ //Allows for 1/2, 1/3, 1/4 speeds ...etc...
			m_speed_counter=0;
			if (m_current_position < m_commanded_position){
				m_direction = STEPPER_UP;
				m_current_position++;
			}
			else{ // <
				m_direction = STEPPER_DOWN;
				m_current_position--;
			}
			shift_step();
			assign_bits();
		}
	}
}

//Function called by the timer interrupt 
void PeachyStepper::microStep(){
	m_microstep_counter=(m_microstep_counter+1)%(STEPPER_U_STEPS+1); //0->STEPPER_U_STEPS inclusive

	if (m_current_position == m_commanded_position){
		//Switch to holding torque mode
		m_direction=STEPPER_STABLE;

		//0 hold torque, always on
		//2 hold torque half on
		//3 hold torque 1/4 on
		if (m_microstep_counter<m_hold_torque){ 
			pins_off();
		}
		//ie: otherwise keep those pins on for the other 3 cycles (0,1,2)
		else{ 
			pins_on();
		}

	}
	else if (m_microstep_counter==0){
		step();
	}
}

void PeachyStepper::setDirection(){
	if (m_current_position == m_commanded_position){
		m_direction=STEPPER_STABLE;
	}
	else {
		if (m_current_position < m_commanded_position){
			m_direction = STEPPER_UP;
		}
		else{ // <
			m_direction = STEPPER_DOWN;
		}
	}
}

void PeachyStepper::shift_step(){
	uint8_t carry_bit;

	if (m_direction == STEPPER_UP){
		carry_bit=m_step_state & MASK_TOPBIT;
		m_step_state=m_step_state<<1; //Shift up
		if (carry_bit==0x80)
			m_step_state+=0x01;//Bring in the carry
	}
	else if(m_direction == STEPPER_DOWN){
		carry_bit=m_step_state & MASK_BOTBIT;
		m_step_state=m_step_state>>1; //Shift down
		if (carry_bit==0x01)
			m_step_state+=0x80;//Add in the carry at the top
	}
}

void PeachyStepper::pins_off(){
	digitalWrite(STEPPER_PIN0,0);
	digitalWrite(STEPPER_PIN1,0);
	digitalWrite(STEPPER_PIN2,0);
	digitalWrite(STEPPER_PIN3,0);
}

void PeachyStepper::pins_on(){
	assign_bits();
}

void PeachyStepper::assign_bits()
{
	// 4 pin servos "on" steps:
	// [pin0 pin1 pin2 pin3]
	// [1 0 0 0] 0 
	// [1 1 0 0] 1 
	// [0 1 0 0] 2 
	// [0 1 1 0] 3 
	// [0 0 1 0] 4 
	// [0 0 1 1] 5 
	// [0 0 0 1] 6 
	// [1 0 0 1] 7 
	uint8_t tmp_bit;

	tmp_bit = (m_step_state & MASK_PIN0);
	digitalWrite(STEPPER_PIN0,tmp_bit);

	tmp_bit = (m_step_state & MASK_PIN1);
	digitalWrite(STEPPER_PIN1,tmp_bit);

	tmp_bit = (m_step_state & MASK_PIN2);
	digitalWrite(STEPPER_PIN2,tmp_bit);

	tmp_bit = (m_step_state & MASK_PIN3);
	digitalWrite(STEPPER_PIN3,tmp_bit);
}

#ifdef _TEST_PEACHYSTEPPER

void digitalWrite(unsigned char a,unsigned char b){} //Stub it out for testing
void delayMicroseconds(int c){} //Stub out arduino delay
#include <stdio.h>

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
	(byte & 0x80 ? 1 : 0), \
(byte & 0x40 ? 1 : 0), \
(byte & 0x20 ? 1 : 0), \
(byte & 0x10 ? 1 : 0), \
(byte & 0x08 ? 1 : 0), \
(byte & 0x04 ? 1 : 0), \
(byte & 0x02 ? 1 : 0), \
(byte & 0x01 ? 1 : 0) 

void print_status(PeachyStepper *Stepper){
	uint8_t stepper_state;

	stepper_state = Stepper->getState();
	printf("Stepper_state: " BYTETOBINARYPATTERN "\n", BYTETOBINARY(stepper_state));
}

void print_positions(PeachyStepper *Stepper){
	int32_t stepper_position;
	int32_t stepper_c_position;
	uint8_t stepper_direction;	

	stepper_c_position=Stepper->getCommandedPosition();
	stepper_direction=Stepper->getDirection();
	stepper_position=Stepper->getPosition();
	printf("Stepper_position: %i\n",stepper_position);
	printf("Stepper_c_position: %i\n",stepper_c_position);
	printf("Stepper_direction: %d\n",stepper_direction);
}

void test_basicSteps(PeachyStepper *Stepper){
	print_status(Stepper);
	printf("\n STEPPING 1 times \n");
	Stepper->move(STEPPER_UP);
	Stepper->step();

	print_status(Stepper);

	printf("\n STEPPING 12 times \n");
	for (int i=0;i<12;i++){
		Stepper->move(STEPPER_UP);
		Stepper->step();
		print_status(Stepper);
	}
}

void test_move(PeachyStepper *Stepper){
	print_status(Stepper);
	print_positions(Stepper);
	printf("\n Moving 12 steps DOWN \n");
	Stepper->move(STEPPER_DOWN,12);
	for (int i=0;i<12;i++){
		Stepper->step();
		print_status(Stepper);
		print_positions(Stepper);
	}
}

void test_stop(PeachyStepper *Stepper){
	printf("\n Moving 5 steps DOWN and stopping \n");
	print_status(Stepper);
	print_positions(Stepper);
	Stepper->move(STEPPER_DOWN,12);
	for (int i=0;i<5;i++){
		Stepper->step();
		print_status(Stepper);
		print_positions(Stepper);
	}
	Stepper->stop();
	print_status(Stepper);
	print_positions(Stepper);
}

void test_ustepping(PeachyStepper *Stepper){
	printf("\n Moving 4 steps UP with microstepping \n");
	print_status(Stepper);
	print_positions(Stepper);

	Stepper->move(STEPPER_UP,4);
	for (int i=0; i<18; i++){
		Stepper->microStep();
		print_status(Stepper);
		print_positions(Stepper);
	}
}

int main(){

	PeachyStepper MyStepper(2);
	print_status(&MyStepper);

	test_basicSteps(&MyStepper);
	test_move(&MyStepper);
	test_stop(&MyStepper);
	test_ustepping(&MyStepper);
	//test_move_zero();


	return 0;
}
#endif






