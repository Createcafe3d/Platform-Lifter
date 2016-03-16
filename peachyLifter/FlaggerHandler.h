#ifndef _FLAGGER_HANDLER
#define _FLAGGER_HANDLER

#include "PeachyDefines.h"
#include "PeachyTimer2Setup.h" //for the global flagger
#include "PeachyMain.h"
#include "SerialHandler.h" //for dripper state
#include <stdint.h>

#define STEPPER_LIMIT_BOUNCEBACK 20

//Quick Examples for steps -> milliseconds
//TICK_TIME is defined in the PeachyDefines.h
#define TICK_1000MS 1/TICK_TIME
#define TICK_100MS	0.1/TICK_TIME
#define TICK_10MS		0.01/TICK_TIME
#define TICK_1MS		0.001/TICK_TIME

#define DRIP_TIME					 (uint16_t)(2 * TICK_100MS) //100ms minimum dead time between drips by design
#define BUTTON_TIME				 (uint16_t)(4 * TICK_10MS)
#define ANALOG_TIME				 (uint16_t)(6 * TICK_10MS)
#define LIMIT_SWITCH_TIME  (uint16_t)(5 * TICK_10MS)

void oneSecondHandler();
void limitSwitchHandler();
void buttonHandler();
void dripHandler();
void analogHeightHandler();

#endif

