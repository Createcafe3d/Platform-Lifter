#ifndef _PEACHY_DEFINES
#define _PEACHY_DEFINES

//#define STEPPER_STROBE //If this is defined use the strobe, else use the 3 pin
#define CLOCK_HACK	 //If this is defined, force the clock up to 16MHz with register writes

//Pin definitions:
#define HEIGHT_ANALOG_PIN A0
#define DRIP_PIN A4
#define CAMERA_PIN A5
#define LED_RED_PIN 6
#define LED_BLUE_PIN 7
#define LED_YELLOW_PIN 8
#define START_BUTTON_PIN 9
#define HEIGHT_BUTTON_PIN 10
#define RESET_BUTTON_PIN 11
#define LIMIT_PIN 12

//Define Print states, as the total machine between layers
#define NUMBER_PRINT_STATES 6
#define PRINT_STATE_PRINTING 0
#define PRINT_STATE_RESURRECTING 1
#define PRINT_STATE_SUBMERGING 2
#define PRINT_STATE_LIFTING 3
#define PRINT_STATE_FLOWING 4
#define PRINT_STATE_PREPRINTING 5

//The digital pins used for the stepper driver
//#define STEPPER_PIN0 2
//#define STEPPER_PIN1 3
//#define STEPPER_PIN2 4
//#define STEPPER_PIN3 5
#define STEPPER_PIN0 2
#define STEPPER_PIN1 3
#define STEPPER_PIN2 4
#define STEPPER_PIN3 5

#define PICTURE_QUICK_TIME  0.1 //seconds
#define PICTURE_QUICK_TICKS PICTURE_QUICK_TIME/TICK_TIME
#define PICTURE_LONG_TIME  0.1 //seconds
#define PICTURE_LONG_TICKS PICTURE_LONG_TIME/TICK_TIME

#define MAX_NUM_FLAGS 16

#define DRIPS_PER_LAYER 2 //Number of drips sent at begining of each layer
#define DRIPS_AT_START 1 //Number of drips to send when starting a print, one time
#define LIMIT_SWITCH_BUFFER_STEPS 500 //Steps down after hitting limit switch

//Drips are defined as three or more: 0->3.3V edges within about 5ms.
//Once a drip is detected there is a ~100ms dead time before it can register another drip
#define DRIP_TOGGLES 8 //Number of half cycles (on-off is 2 half cycles) per a drip

//Must be an integer less than 64 Because:(10 bit a/d * ANALOG_SCALER) -> uint16
//How much to multiply up the Analog reading by. Analog values range from 0->1023
#define ANALOG_SCALER 3 //Default 3

//Global states:
#define STATE_LIMITED 2
#define STATE_ANALOG 1
#define STATE_NORMAL 0
#define STEPPER_STABLE 2
#define STEPPER_UP 1
#define STEPPER_DOWN 0

//Default values:
//0 -> Full strengh holding torque. 
//1 -> Coils OFF for 1/4 of the time when stable
//3->  Coils ON for 1/4 of the time when stable
//4->  Coils OFF when not moving
//holding torque strength as number of "off" microsteps (out of total micro steps)
#define STEPPER_HOLD_TORQUE 4
// How many times step gets called (minus one) between actual motor steps
// These steps include the 0th step,
// ie: 3 means 0->1->2->3 (4 calls)
#define STEPPER_U_STEPS 3 //Must be greater than 0

// Setting up the Timer Interrupt for a specific "tick" time:
//16MHz clock, with prescaler of 64
//To get a "tick" time of 200us we do:
// <time per tick> * <Clock frequency> / <prescaler>
// 200e-6 * 16e6 / 64 = 50
// WARNING: that number must be between 0-255
#define CPU_FREQ 16e6 //Hz
#define TICK_TIME 250e-6 //seconds
#define TIM2_PRESCALER 64 //Dependant on the setupTIM2_ISR() function settings
#define TIM2_START (uint8_t)(256-TICK_TIME*CPU_FREQ/TIM2_PRESCALER) //Must be less than 256
//Example:
//Flag Tick time == 200us (200e-6)
// 5000 ticks == 1 second
// 500 ticks == 100ms

//Set Baud to match. Default: 9600
#define SERIAL_BAUD					9600 //bits/s
#define SERIAL_BUFFER_BYTES 64
#define SERIAL_BUFFER				SERIAL_BUFFER_BYTES*8 //bits

#define SERIAL_STARVE_TICKS		(uint32_t)((SERIAL_BAUD/SERIAL_BUFFER)/TICK_TIME)
#define SERIAL_STARVE_MARGIN	(SERIAL_STARVE_TICKS/64)/2 //32 bytes of margin
#define SERIAL_STARVE_MAX			SERIAL_STARVE_TICKS-SERIAL_STARVE_MARGIN
#define SERIAL_BYTE_TICKS			SERIAL_STARVE_TICKS/SERIAL_BUFFER_BYTES

#define MICROMETERS_PER_LAYER 200 //my best guess - Will
#define MICROMETERS_PER_STEP  (double)(25.4 / 2) //two microsteps per 1 thou (inch) - Rylan
#define MILLIMETERS_PER_STEP	(double)(MICROMETERS_PER_STEP/1e3)
#define STEPS_PER_LAYER				MICROMETERS_PER_LAYER/MICROMETERS_PER_STEP

#endif


