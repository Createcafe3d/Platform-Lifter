#ifndef _SERIAL_HANDLER
#define _SERIAL_HANDLER

#define ON 1
#define OFF 0

uint8_t g_dripper_state = OFF;
uint8_t g_layer_state = OFF;
double  g_layer_float = 0.0;

void serialDrip(uint8_t state){
	g_dripper_state=state;
}

void serialLayer(uint8_t state){
	g_layer_state=state;
}

void serialPrintDone(){
	//dunno what to do here? return to zero?
	findUpperLimit();
}

void nextLayer(){
	g_layer_float-=STEPS_PER_LAYER;
	Serial.println(g_layer_float);
	Serial.println(STEPS_PER_LAYER);
	g_Stepper.moveTo((int32_t)g_layer_float);
}

void sendHello(){
	Serial.write("OK\n");
}

// This gets called if there are SERIAL_NUMBYTES_TRIGGER characters in the buffer
// ... or it would if the arduino documentation was accurate
void serialEvent(){
	uint8_t serial_data;

	while (Serial.available()){
		serial_data=Serial.read();
		//Serial.write(serial_data); //write is faster than print, if the data is already a char
		switch(serial_data){
			case '7':
				//DRIP ON
				serialDrip(ON);
				break;
			case '8':
				//DRIP OFF
				serialDrip(OFF);
				break;
			case 'S':
				//LAYER STARTED
				serialLayer(ON);
				break;
			case 'E':
				//LAYER ENDED
				serialLayer(OFF);
				//break;
			case 'H':
				//NEXT LAYER
				nextLayer();
				break;
			case 'D':
				//she wants the D
				sendHello();
				break;
			case 'Z':
				serialPrintDone();
				//PRINT ENDED
				break;
		}
	}
	//Serial.write('\n'); //Newline to show what we all got in one function call
}

#endif
