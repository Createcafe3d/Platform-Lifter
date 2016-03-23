#include "Arduino.h"
#include <stdint.h>

#include "PeachyDefines.h"
#include "PeachyMain.h"

//g_Flagger and g_Stepper are found in PeachyTimer2Setup.h
#include "PeachyTimer2Setup.h"
#include "SerialHandler.h"
#include "FlaggerHandler.h"
#include "PeachyPrintState.h"

#ifdef STEPPER_STROBE
	extern PeachyStrobeStepper g_Stepper;
#else
	extern PeachyStepper g_Stepper;
#endif

//externs
extern PeachyFlagger g_Flagger;
extern PeachyPrintState g_PrintState;

extern uint16_t g_interrupt_count;
extern uint16_t g_Serial_starved_count;
extern uint8_t g_Serial_starved;
extern int32_t g_resin_height;

extern uint8_t g_drips_requested;

double g_layer_float;

void setup()
{
  Serial.begin(SERIAL_BAUD);
  pinMode(DRIP_PIN,OUTPUT);
  pinMode(CAMERA_PIN,OUTPUT);
	pinMode(LED_RED_PIN,OUTPUT); 
	pinMode(LED_BLUE_PIN,OUTPUT); 
	pinMode(LED_YELLOW_PIN,OUTPUT); 
  pinMode(START_BUTTON_PIN,INPUT_PULLUP);
  pinMode(HEIGHT_BUTTON_PIN,INPUT_PULLUP);
  pinMode(RESET_BUTTON_PIN,INPUT_PULLUP);
	pinMode(LIMIT_PIN,INPUT_PULLUP); 
	pinMode(STEPPER_PIN0,OUTPUT);
	pinMode(STEPPER_PIN1,OUTPUT);
	pinMode(STEPPER_PIN2,OUTPUT);
	pinMode(STEPPER_PIN3,OUTPUT);

	noInterrupts();
	setupTIM2_ISR();
  interrupts();

  findUpperLimit();
	g_Stepper.setSpeed(1); //1/X speed, where X is the argument
	g_PrintState.setResinHeight(g_resin_height); //figured out by findUpperLimit;
	g_PrintState.start(PRINT_STATE_PRINTING);//Choose starting state
	g_drips_requested=1;

	initialize_flags();
	initializePrintStates();
}

void loop()
{
  uint16_t tmp_count;
	//debug function:
  if (g_interrupt_count>5000){
    tmp_count=g_interrupt_count;
    Serial.write("5000+\n");
    g_interrupt_count=0;
  }

	//Handler functions found in FlaggerHandler.h
	limitSwitchHandler();
  dripHandler();
  buttonHandler();
  analogHeightHandler();
	oneSecondHandler();

	//Handle the print state machine
	g_PrintState.handlePrintStates();
}

//***********************************
//*** Extra Functions ***************
//***********************************



//This function gets called every TICK_TIME
ISR(TIMER2_OVF_vect){
  TCNT2=TIM2_START; //Reset the timer to start value for consistant roll overs
  g_interrupt_count++;//my favourite debug counter
	g_Serial_starved_count++;
	g_Flagger.tick();
  g_Stepper.microStep();
	serialCheckStarved();
  //Interrupt Flag cleared automagically
}

void initializePrintStates(){
	g_PrintState.initializeStateDistanceTime(PRINT_STATE_PRINTING,
			5.0,//seconds delay
			-10.0, //mm from resin
			true, //photo during delay (true|false)
			false, //photo before delay (true|false)
			false, //photo after delay (true|false)
			true);//wait for an external trigger? (ie: layer done message)

	g_PrintState.initializeStateDistanceTime(PRINT_STATE_RESURRECTING,
			5.0,//seconds delay
			-20.0, //mm from resin
			false, //photo during delay (true|false)
			true, //photo before delay (true|false)
			true, //photo after delay (true|false)
			false);//wait for an external trigger? (ie: layer done message)
	g_PrintState.initializeStateDistanceTime(PRINT_STATE_SUBMERGING,
			0.5,//seconds delay
			0.0, //mm from resin
			false, //photo during delay (true|false)
			true, //photo before delay (true|false)
			true, //photo after delay (true|false)
			false);//wait for an external trigger? (ie: layer done message)
	g_PrintState.initializeStateDistanceTime(PRINT_STATE_LIFTING,
			0.5,//seconds delay
			-40.0, //mm from resin
			false, //photo during delay (true|false)
			false, //photo before delay (true|false)
			false, //photo after delay (true|false)
			false);//wait for an external trigger? (ie: layer done message)
	g_PrintState.initializeStateDistanceTime(PRINT_STATE_FLOWING,
			5.0, //seconds delay
			-30.0, //mm from resin
			false, //photo during delay (true|false)
			false, //photo before delay (true|false)
			false, //photo after delay (true|false)
			false);//wait for an external trigger? (ie: layer done message)
	g_PrintState.initializeStateDistanceTime(PRINT_STATE_PREPRINTING,
			5.0,//seconds delay
			0.0, //mm from resin
			false, //photo during delay (true|false)
			true, //photo before delay (true|false)
			false, //photo after delay (true|false)
			false);//wait for an external trigger? (ie: layer done message)

	//g_PrintState.printStates();
}

void findUpperLimit(){
  uint8_t stepper_direction;

  while(digitalRead(LIMIT_PIN)){
		serialEvent();
    if (g_Stepper.getDirection() == STEPPER_STABLE){
      g_Stepper.move(STEPPER_UP);
    }
  }
  g_Stepper.move(STEPPER_DOWN,LIMIT_SWITCH_BUFFER_STEPS);
  g_Stepper.waitForMove();
  g_Stepper.zeroPosition();
  goToNewStartHeight();
}

void printSetups(){
  Serial.print("tim2_start is:");
  Serial.println(TIM2_START);
  Serial.print("prescaler is:");
  Serial.println(TIM2_PRESCALER);
}

void goToNewStartHeight()
{
  uint16_t analog_result;
  analog_result = analogRead(HEIGHT_ANALOG_PIN);
	g_resin_height = (0-(int32_t)analog_result*ANALOG_SCALER);
	g_layer_float = g_resin_height;
	g_PrintState.setResinHeight(g_resin_height);
  g_Stepper.moveTo(g_resin_height); //0 minus so that we travel DOWN to absolute positions, relative to 0
	g_PrintState.start(0); //Start over
}

