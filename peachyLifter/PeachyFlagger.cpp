#include "PeachyFlagger.h"

PeachyFlagger::PeachyFlagger(){
	for (int i=0; i<MAX_NUM_FLAGS; i++){
		m_flags[i].trig_count=0;	
		m_flags[i].flag_state=0;	
		m_flags[i].current_count=0;
		m_flags[i].enabled=0;	
	}
	m_num_flags=0;
};

uint8_t PeachyFlagger::registerFlag(uint16_t trig_count){
	uint8_t flag_id=m_num_flags;
	m_flags[m_num_flags].trig_count=trig_count;	
	m_flags[m_num_flags].current_count=0;	
	m_flags[m_num_flags].flag_state=0;	
	m_flags[m_num_flags].enabled=1;	
	m_num_flags++;
	return flag_id;
}

void PeachyFlagger::updateTrigCount(uint8_t id, uint16_t new_trig_count){
	m_flags[id].flag_state=0;
	m_flags[id].current_count=0;
	m_flags[id].trig_count=new_trig_count;
}

void PeachyFlagger::clearFlag(uint8_t id){
	m_flags[id].flag_state=0;
}

void PeachyFlagger::decrimentFlag(uint8_t id){
	if (m_flags[id].flag_state)
		m_flags[id].flag_state--;
}

void PeachyFlagger::tick(){
	int i=0;
	while (m_flags[i].enabled){
		m_flags[i].current_count++;
		if (m_flags[i].current_count==m_flags[i].trig_count){
			m_flags[i].flag_state++;
			m_flags[i].current_count=0;
		}
		i++;
	}
}


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
