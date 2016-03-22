#ifndef _PEACHY_PRINTSTATE
#define _PEACHY_PRINTSTATE


#define PICTURE_QUICK_TIME  0.1 //seconds
#define PICTURE_QUICK_TICKS PICTURE_QUICK_TIME/TICK_TIME

#define PICTURE_LONG_TIME  10 //seconds
#define PICTURE_LONG_TICKS PICTURE_LONG_TIME/TICK_TIME

#include <stdint.h>
#include "PeachyDefines.h"
#include "PeachyTimer2Setup.h"
#include "FlaggerHandler.h" //for the defines

//Print State variables with defaults, per each motor movement
//delay_ticks max == 65535
//TICK_TIME == ~200e-6 seconds
//Max Flagger Time == TICK_TIME * delay_ticks_max =>
struct PeachyStates{
	uint32_t delay_ticks = 0;		//ticks delay before doing next state
	int32_t absoluteHeight_steps = 0;			//Absolute height of this state
	uint8_t photoBeforeDelay = false;
	uint8_t photoAfterDelay = false;
	uint8_t photoDuringDelay = false;
	uint8_t externalTrigger = false;
};

class PeachyPrintState
{
	public:
		PeachyPrintState();

		//Seconds and Millimeters
		void initializeStateDistanceTime(uint8_t state, float delay=0.0, float height_from_resin=0, uint8_t photoDuringDelay=false, uint8_t photoBeforeDelay=false, uint8_t photoAfterDelay=false, uint8_t externalTrigger=false);

		//Ticks and Steps
		void initializeState(uint8_t state, uint16_t delay=0, int32_t height=0,uint8_t photoDuringDelay=false, uint8_t photoBeforeDelay=false, uint8_t photoAfterDelay=false, uint8_t externalTrigger=false);

		void initializeFlags();
		void takePicture();
		void takeDuringPicture();
		void setResinHeight(int32_t);
		void pictureHandler();
		void handleFinishedPrintState();
		void handleStartPrintState();
		void handlePrintStates();
		void printStates();//debug
		void externalTrigger(){m_external_triggered=true;}
		void start(uint8_t);
		void stop();
		void start();

		//Steps to print:
		//1) DRAWING: draw a layer
		//2) Raise print all the way up - Pictures
		//3) Totally submerge print - Pictures
		//4) Bring print to printing height + Margin1 (This is to drain off resin that is globbed on top) - Pictures
		//5) Bring print to printing height - Margin2 (This is to allow the surface tension to flow back over) - Pictures
		//6) Bring print to printing height (This is to allow the surface tension to flow back over) - Pictures (timelapse mode optional)
		//
		//Print -> Resurrect -> Submerge -> Lift -> Flow -> Print

	private:

		uint8_t m_flagger_id_state;
		uint8_t m_flagger_id_picture;
		uint8_t m_picture_pin_state;
		uint8_t m_printState;
		uint8_t m_finished_state=false;
		uint8_t m_external_triggered=false; //cleared each time we change state
		int32_t m_resin_height_steps;

		float m_layerHeight_mm;       //mm per layer
		uint16_t m_layerHeight_steps;

		PeachyStates m_printStates[NUMBER_PRINT_STATES];
};
#endif
