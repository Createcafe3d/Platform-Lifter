#ifndef _FLAGGER_H
#define _FLAGGER_H

#include <stdint.h>
#include "PeachyDefines.h"

struct Flag_info{
	uint16_t trig_count;
	uint16_t current_count;
	uint8_t flag_state;	
	uint8_t enabled;
};

class PeachyFlagger
{
public:
	PeachyFlagger();

	uint8_t registerFlag(uint16_t trig_count);
	void updateTrigCount(uint8_t id, uint16_t new_trig_count);
	void clearFlag(uint8_t id);

	void decrimentFlag(uint8_t id);
	void tick();

	void enable(uint8_t id);
	void disable(uint8_t id);

	uint8_t getFlag(uint8_t id){ return m_flags[id].flag_state; }

private:
	Flag_info m_flags[MAX_NUM_FLAGS];
	uint8_t m_num_flags;
};

#endif
