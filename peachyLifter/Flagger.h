#ifndef _FLAGGER_H
#define _FLAGGER_H

#include <stdint.h>
#define MAX_NUM_FLAGS 8


struct Flag_info{
	uint8_t id;
	uint16_t trig_count;
	uint8_t flag_state;	
};

class Flagger
{
	public:
		Flagger(){
			for (int i=0; i<MAX_NUM_FLAGS; i++){
				m_flags[i].id=0;	
				m_flags[i].trig_count=0;	
				m_flags[i].flag_state=1;	
			}
		m_num_flags=0;
		m_max_flags=MAX_NUM_FLAGS;
		};

		uint8_t registerFlag(uint16_t trig_count){
			m_flags[m_num_flags].id=m_num_flags;	
			m_flags[m_num_flags].trig_count=trig_count;	
			m_flags[m_num_flags].flag_state=0;	
			m_num_flags++;
		}

		uint8_t getFlag(uint8_t id){
			return m_flags[id].flag_state;
		}

	private:
		Flag_info m_flags[MAX_NUM_FLAGS];
		uint8_t m_num_flags;
		uint8_t m_max_flags;
};

#endif
