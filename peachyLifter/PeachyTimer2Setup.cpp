#include "PeachyTimer2Setup.h"

//Reference for taking control of Timer2 and fixing the Uno's clock so it actually runs at 16MHz
//Register definitions found Page ~157 in datasheet
//TCNT2   //Timer/Counter Register. The actual timer value is stored here.
//TCCR2B  //Timer Control Register (Prescaler here)
//TIMSK2  //Timer Interrupt Mask Register
//CLKPR		//Clock initialization Register
uint16_t g_interrupt_count=0;
uint16_t g_Serial_starved_count=0;
uint8_t g_Serial_starved=0;

PeachyStepper g_Stepper(STEPPER_HOLD_TORQUE);
PeachyFlagger g_Flagger;
PeachyPrintState g_PrintState;

void setupTIM2_ISR(){

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

void serialCheckStarved(){
	if (g_Serial_starved_count>SERIAL_STARVE_MAX){
		g_Serial_starved=1;
		g_Serial_starved_count++;
	}
	else
		g_Serial_starved=0;
}
