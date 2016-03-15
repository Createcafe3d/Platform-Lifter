
#include "PeachyFlagger.h"
#include "PeachyStepper.h"


uint16_t g_interrupt_count=0;
PeachyFlagger g_Flagger;

//0 -> Full strenght holding torque. 3-> 1/4 holding torque. 4-> coils off when not moving
PeachyStepper g_Stepper(0); //holding torque strength as number of "off" microsteps (out of total micro steps)

void setupTIM2_ISR(){
	//Register definitions found Page ~157 in datasheet
	//TCNT2   //Timer/Counter Register. The actual timer value is stored here.
  //TCCR2A  //Timer Output comare register for setting interrupt values in non-roll over mode (not used)
  //TCCR2B  //Timer Control Register (Prescaler here)
	//TIMSK2  //Timer Interrupt Mask Register
	//CLKPR		//Clock initialization Register

  //Make the chip run at 16MHz like it should be -_- thanks arduino
  CLKPR = 0; //Clock Prescaller OFF and set to 1:1 clock speed
  CLKPR |= 1 << CLKPCE; //Start the Change by setting a 1 in CLKPCE
  CLKPR = 0; //zero the CLKPCE register to force a reset of clk sources (upgraaade!)

  //Timer2 Prescaler is set as follows [CS22, CS21, CS20]:
  // 000 - Stopped
  // 001 - no prescaler
  // 010 - divide by 8
  // 011 - divide by 32
  // 100 - divide by 64
  // 101 - divide by 128
  // 110 - divide by 256
  // 111 - divide by 1024
  TCCR2B |= ((1 << CS22) | (0 << CS21) | (0 << CS20)); //64
  TCCR2A = 0; //Normal operation
  
  TIMSK2 |= (1 << TOIE2); //Enable the Timer2 interrupt
  TCNT2=TIM2_START; //Preload it to the correct time for consistent roll overs
}

//This function gets called every TICK_TIME
ISR(TIMER2_OVF_vect){
  TCNT2=TIM2_START; //Reset the timer to start value for consistant roll overs
  g_interrupt_count++;
	g_Flagger.tick();
  g_Stepper.microStep();
  //Flag cleared automagically
}
