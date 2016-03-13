
//16MHz clock, with prescaler of 64
//To get a "tick" time of 200us we do:
// <time per tick> * <Clock frequency> / <prescaler>
// 200e-6 * 16e6 / 64 = 50
#define CPU_FREQ 16e6 //MHz
#define TICK_TIME 200e-6 //seconds
#define TIM2_PRESCALER 64 //Dependant on the setupTIM2_ISR() function
#define TIM2_START (uint8_t)(256-TICK_TIME*CPU_FREQ/TIM2_PRESCALER)

uint16_t g_interrupt_count=0;
Flagger g_Flagger;
PeachyStepper g_Stepper;

void setupTIM2_ISR(){
	//Register definitions found Page ~157 in datasheet
	//TCNT2   //Timer/Counter Register. The actual timer value is stored here.
  //TCCR2B  //Timer Control Register (Prescaler here)
	//TIMSK2  //Timer Interrupt Mask Register
 
  //Prescaler is set as follows [CS22, CS21, CS20]:
  // 000 - Stopped
  // 001 - no prescaler
  // 010 - divide by 8
  // 011 - divide by 32
  // 100 - divide by 64
  // 101 - divide by 128
  // 110 - divide by 256
  // 111 - divide by 1024
  TCCR2B |= ((1 << CS22) | (0 << CS21) | (0 << CS20)); //64
  TIMSK2 |= (1 << TOIE2); //Enable the Timer2 interrupt
  TCNT2=TIM2_START; //Preload it to the correct time for consistent roll overs
}

ISR(TIMER2_OVF_vect){
  TCNT2=TIM2_START; //Reset the timer to start value for consistant roll overs
  g_interrupt_count++;
	g_Flagger.tick();
  g_Stepper.micro_step();
  //Flag cleared automagically
}
