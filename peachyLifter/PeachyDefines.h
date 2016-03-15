
#define LED_PIN 7
#define LIMIT_PIN 12
#define DRIP_PIN A4
#define RESET_BUTTON_PIN 11
#define HEIGHT_BUTTON_PIN 10
#define HEIGHT_ANALOG_PIN A0

#define DRIP_TOGGLES 8 //Number of half cycles (on-off is 2 half cycles)
#define LIMIT_SWITCH_BUFFER_STEPS 500

#define ANALOG_SCALER 3 //How much to multiply up the Analog reading by. Analog values range from 0->1023

#define STATE_ANALOG 1
#define STATE_NORMAL 0

//Flag Tick time == 200us
// 5000 == 1 second
// 500 == 100ms
