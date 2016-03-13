#include "PeachyFlagger.h"

/*
PeachyFlagger::registerFlag(uint16_t trig_count){

}
*/

#ifdef _TEST_FLAGGER
//Test Code
#include <iostream>

int main()
{
	using namespace std;
	uint8_t tmp_flag;
	uint8_t flag_id, flag_id2;
	uint8_t flag_status;

	cout << "Hello World" << endl;
	PeachyFlagger MyFlagger;
	flag_id = MyFlagger.registerFlag(20);
	cout << "flag_id: "<< (int)flag_id << endl;
	flag_id2 = MyFlagger.registerFlag(30);
	cout << "flag_id2: "<< (int)flag_id2 << endl;

	flag_status = MyFlagger.getFlag(flag_id);
	cout << "Flag before ticks: "<< (int)flag_status << endl;
	//tick 19 times
	for (int i=0;i<19;i++){
		MyFlagger.tick();
	}

	flag_status = MyFlagger.getFlag(flag_id);
	cout << "Flag before 20th tick: "<< (int)flag_status << endl;
	MyFlagger.tick();
	flag_status = MyFlagger.getFlag(flag_id);
	cout << "Flag after 20th tick: "<< (int)flag_status << endl;

	

	




}

#endif
