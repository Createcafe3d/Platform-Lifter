#ifndef _PEACHY_TIMER2_ISR
#define _PEACHY_TIMER2_ISR

#include "PeachyDefines.h"
#include "PeachyFlagger.h"
#include "PeachyStepper.h"
#include "Arduino.h"
#include <stdint.h>


void setupTIM2_ISR();
void serialCheckStarved();

extern PeachyStepper g_Stepper;

#endif
