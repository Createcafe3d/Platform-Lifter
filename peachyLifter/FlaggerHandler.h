#ifndef _FLAGGER_HANDLER
#define _FLAGGER_HANDLER

#include "PeachyDefines.h"

uint8_t g_1000ms_flag = g_Flagger.registerFlag(5000);
uint8_t g_drip_flag = g_Flagger.registerFlag(1000);
uint8_t g_buttons_flag = g_Flagger.registerFlag(300);
uint8_t g_analog_flag = g_Flagger.registerFlag(2000);

void buttonHandler(){
  if (g_Flagger.getFlag(g_buttons_flag)){
    if (digitalRead(RESET_BUTTON_PIN) == 0){
      g_Stepper.stop();
      findUpperLimit();
    }
    if (digitalRead(HEIGHT_BUTTON_PIN) == 0){
      g_Stepper.stop();
      g_system_state=STATE_ANALOG;
    }
    g_Flagger.clearFlag(g_buttons_flag);
  }
}

void dripHandler(){

  if (g_Flagger.getFlag(g_drip_flag)){
    if (g_drips_requested > 0){
      g_drips_requested--;
      for (uint8_t i=0; i<DRIP_TOGGLES; i++){
        digitalWrite(DRIP_PIN,1);
        delay(1);
        digitalWrite(DRIP_PIN,0);
      }
      g_Flagger.clearFlag(g_drip_flag);
    }
  }
}

void analogHeightHandler(){
  if (g_system_state==STATE_ANALOG){
    findUpperLimit(); //blocking
    while(g_system_state==STATE_ANALOG){
      goToNewStartHeight();
      while (digitalRead(HEIGHT_BUTTON_PIN)==0){
        g_system_state=STATE_NORMAL;
        findUpperLimit();
      }
    }
  }
}

#endif

