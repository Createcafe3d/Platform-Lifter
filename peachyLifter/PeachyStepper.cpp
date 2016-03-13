#include <stdint.h>

#ifdef _TEST_PEACHYSTEPPER

void digitalWrite(unsigned char,unsigned char){} //Stub it out for testing

#include "PeachyStepper.h"

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
	printf("Stepper_state: "BYTETOBINARYPATTERN"\n", BYTETOBINARY(stepper_state));
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
		Stepper->micro_step();
		print_status(Stepper);
		print_positions(Stepper);
	}
	

}

int main(){

	PeachyStepper MyStepper;
	print_status(&MyStepper);

	test_basicSteps(&MyStepper);
	test_move(&MyStepper);
	test_stop(&MyStepper);
	test_ustepping(&MyStepper);
	//test_move_zero();

	
	return 0;
}





#endif






