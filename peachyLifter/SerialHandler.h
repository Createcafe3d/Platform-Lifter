#ifndef _SERIAL_HANDLER
#define _SERIAL_HANDLER

#include "PeachyTimer2Setup.h" //for g_Stepper
#include "PeachyMain.h" //

#define ON 1
#define OFF 0

void serialDrip(uint8_t state);
void serialLayer(uint8_t state);
void serialPrintDone();
void nextLayer();

void sendHello();
void handleChar();
void serialEvent();

#endif
