
#define LED_RED_PIN 6
#define LED_BLUE_PIN 7
#define LIMIT_PIN 12
#define DRIP_PIN A4
#define RESET_BUTTON_PIN 11
#define START_BUTTON_PIN 9
#define HEIGHT_BUTTON_PIN 10
#define HEIGHT_ANALOG_PIN A0

#define DRIP_TOGGLES 8 //Number of half cycles (on-off is 2 half cycles)
#define LIMIT_SWITCH_BUFFER_STEPS 500

#define ANALOG_SCALER 3 //How much to multiply up the Analog reading by. Analog values range from 0->1023

#define STATE_ANALOG 1
#define STATE_NORMAL 0

// Setting up the Timer Interrupt for a specific "tick" time:
//16MHz clock, with prescaler of 64
//To get a "tick" time of 200us we do:
// <time per tick> * <Clock frequency> / <prescaler>
// 200e-6 * 16e6 / 64 = 50
// WARNING: that number must be between 0-255
#define CPU_FREQ 16e6 //Hz
#define TICK_TIME 500e-6 //seconds
#define TIM2_PRESCALER 64 //Dependant on the setupTIM2_ISR() function settings
#define TIM2_START (uint8_t)(256-TICK_TIME*CPU_FREQ/TIM2_PRESCALER) //Must be less than 256
//Example:
//Flag Tick time == 200us (200e-6)
// 5000 ticks == 1 second
// 500 ticks == 100ms
