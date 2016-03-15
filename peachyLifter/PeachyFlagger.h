#ifndef _FLAGGER_H
#define _FLAGGER_H

#include <stdint.h>
#define MAX_NUM_FLAGS 8


struct Flag_info{
	uint16_t trig_count;
	uint16_t current_count;
	uint8_t flag_state;	
	uint8_t enabled;
};

class PeachyFlagger
{
	public:
		PeachyFlagger(){
			for (int i=0; i<MAX_NUM_FLAGS; i++){
				m_flags[i].trig_count=0;	
				m_flags[i].flag_state=0;	
				m_flags[i].current_count=0;
				m_flags[i].enabled=0;	
			}
		m_num_flags=0;
		};

		uint8_t registerFlag(uint16_t trig_count){
			uint8_t flag_id=m_num_flags;
			m_flags[m_num_flags].trig_count=trig_count;	
			m_flags[m_num_flags].current_count=0;	
			m_flags[m_num_flags].flag_state=0;	
			m_flags[m_num_flags].enabled=1;	
			m_num_flags++;
			return flag_id;
		}

		void updateTrigCount(uint8_t id, uint16_t new_trig_count){
			m_flags[id].flag_state=0;
			m_flags[id].current_count=0;
			m_flags[id].trig_count=new_trig_count;
		}

		void clearFlag(uint8_t id){
			m_flags[id].flag_state=0;
		}

		void decrimentFlag(uint8_t id){
			if (m_flags[id].flag_state)
				m_flags[id].flag_state--;
		}

		void tick(){
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

		uint8_t getFlag(uint8_t id){
			return m_flags[id].flag_state;
		}

	private:
		Flag_info m_flags[MAX_NUM_FLAGS];
		uint8_t m_num_flags;
};

#endif
