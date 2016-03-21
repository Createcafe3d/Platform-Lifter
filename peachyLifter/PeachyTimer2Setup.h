#ifndef _PEACHY_TIMER2_ISR
#define _PEACHY_TIMER2_ISR

#include "PeachyDefines.h"
#include "PeachyFlagger.h"
#include "PeachyStepper.h"
#include "PeachyPrintState.h"
#include "Arduino.h"
#include <stdint.h>


void setupTIM2_ISR();
void serialCheckStarved();

//In your main function, you need to include the ISR(TIMER2_OVF_vect){} function

#endif
