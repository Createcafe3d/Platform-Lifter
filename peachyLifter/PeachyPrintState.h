#ifndef _PEACHY_PRINTSTATE
#define _PEACHY_PRINTSTATE

#include <stdint.h>
class PeachyPrintState
{
	public:
		PeachyPrintState(){

		}

	private:
		float layerHeight = .16;        // bigDip: 0.1   , bounceyDip
		float dipPrintHeight = 5;    // bigDip: 6.0   , bounceyDip
		int liftPrintHeight = 3.0;       // bigDip: 3.    , bounceyDip
		int delayAfterLift = 4000;      // bigDip: 2000  , bounceyDip
		int delayAtSurface = 3000;        // bigDip: 3000     , bounceyDip
		int secondDipHeight = .2;
		boolean reserectPhoto = true;
		boolean justArivedAtSurfacePhoto = true;
		boolean afterDelayAtSurfacePhoto = true;
		boolean afterLiftPhoto = true; 
		boolean afterLiftDelayPhoto = true;
		int delayAfterReserect = 5000;


#endif