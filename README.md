# Platform-Lifter
Stepper motor controlled print platform - Controlled by an arduino

##Usage
There are 2 classes that do all of the heavy lifting (har har). They are:

###PeachyFlagger
Setup defines found in PeachyStepper.h:
```cpp
#define MAX_NUM_FLAGS 8
```

The public functions are as follows:
```cpp
MyFlagger = PeachyFlagger(); //Default max 8 flags defined in the header

//To register a flag, see below. Internally it checks it's own counters and sets the flag after "trigger_count" ticks
uint8_t flag_id = MyFlagger.registerFlag(trigger_count); //A tick is a single Timer2 interrupt (default 200us)

uint8_t MyFlagger.getFlag(flag_id); //returns the flag variable as a count of how many times that flag has been raised
MyFlagger.clearFlag(flag_id); //Clear the flags once you dealt with it
MyFlagger.decrimentFlag(flag_id); //Useful for if flags pile up and you need to do something each time

MyFlagger.tick(); //time tick, usually called inside the timer2 interrupt
```

###PeachyStepper
Stepper position is given as a 32 bit signed int which begins at 0. 

Setup defines found in PeachyStepper.h:
```cpp

//The digital pins used for the stepper driver
#define STEPPER_PIN0 8
#define STEPPER_PIN1 9
#define STEPPER_PIN2 10
#define STEPPER_PIN3 11

#define STEPPER_STABLE 2
#define STEPPER_UP 1
#define STEPPER_DOWN 0
```

The public functions are as follows:
```cpp

MyStepper = PeachyStepper(); //Defaulting to 4 microsteps per motor step
MyStepper = PeachyStepper(2); //2 microsteps per motor step

MyStepper.getPosition(); //get Absolute motor position as a 32 bit int
MyStepper.getCommandedPosition(); //get the position we are moving towards

MyStepper.move(STEPPER_UP|STEPPER_DOWN) //Move one motor step
MyStepper.move(STEPPER_UP|STEPPER_DOWN, number_of_steps); //Move a bunch of motor steps
MyStepper.moveTo(stepper_position); //Move to a specific motor step position

MyStepper.stop(); //Stops motor and clears any move commands

MyStepper.zeroPosition(); //Set current position as the new zero position
MyStepper.step(); //motor step, called if you want to step the motor yourself
MyStepper.micro_step(); //micro step (holding torque only), usually called in the timer interrupt
```

##Default Config
A default config is found in the file PeachyTimer2Interrupt.h containing a global flagger and stepper instance:
```cpp
PeachyFlagger g_Flagger();
PeachyStepper g_Stepper();
```

That you can use anywhere in you code like:
```cpp
#include PeachyTimer2Interrupt.h

//... other codes ...

//Flagger Instance
myFlag_id = g_Flagger.registerFlag(20);
myFlag = g_Flagger.getFlag(myFlag_id);

//Stepper Instance
g_stepper.move(STEPPER_UP) //Move up one step
g_stepper.move(STEPPER_UP,500) //Move up 500 steps
int32_t positionA = g_stepper.getPosition();
g_stepper.moveTo(positionA);
```

