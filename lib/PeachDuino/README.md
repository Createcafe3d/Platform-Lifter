# PeachDuino
This software is provided as is and is not guarenteed to work with all versions of the software. We intend this as a work in progress and hope to expand its functionality to support then many amazing ideas of our users. There are 2 version of this api Advanced and Simple, the advanced version of this api is designed with experienced programmers in mind where as the simple API is diesgned more for hobbiests.

## Simple PeachDuino
The simple peachy duino API is simply a wrapper for the Advanced API below. It functions a little bit slower and uses more memory it does this to provide an easy to use interface to interact with the peachy arduino software.

###Basic Usage
This example with set pin 8 to HIGH when printer is waiting for a drip.

```c++
#include "simplepeachduino.h"

#define WAITING_PIN 8

SimplePeachDuino* peachDuino = new SimplePeachDuino(Serial);  //This creates the API you use to interact with the printer

void setup() 
{
  Serial.begin(9600);  //You must still create a serial connection, ensure the speed matches 
  pinMode(WAITING_PIN, OUTPUT);
}

void loop(void)
{
  peachDuino->process();    //You must call the process method regularly to ensure that all messages are processed.
  if (peachDuino->waitingForNextLayerHeight)
  {
    digitalWrite(WAITING_PIN, HIGH);
  } else {
    digitalWrite(WAITING_PIN, LOW);
  }
}
```

###Properties
* unsigned int targetHeightMicrometer    _(This is the requested height the software would like the printer to go to.)_
* unsigned int currentHeightMicrometer   _(This is the current height the software believes the print to be at.)_
* bool waitingForNextLayerHeight         _(True if the printer is paused waiting for the height of the print to be correct.)_
* unsigned int targetDripCount           _(This is the number of drips the printer is expecting [Use as alternative to targetHeightMicrometer].)_
* short printStatus                      _(The status of the current print as defined below.)_
  * 0 - Unstarted
  * 1 - Printing
  * 2 - Waiting For Next Layer Height
  * 3 - Complete

###Methods

####Communication
* void sendHeightMicrometer(unsigned int height) _(Used to tell the printer software the current height in Micrometers)_
* void sendDripCount(unsigned int drips) _(Used to tell the printer software the current number of drips [Use as alternative to sendHeightMicrometer])_

####Helper
* bool process() _(This method process data in the serial buffer and should be called very regularly Returns True if data read.)_
* unsigned int success() _(Number of successfullly recieved messages.)_
* unsigned int fails() _(Number of failed messages. This may not include messages that did not arrive.)_
* unsigned long recieved() _(Number of bytes recieved.)_
* unsigned long sent() _(Number of bytes sent.)_


###Common Problems

#### Delay and DelayMicroseconds
A Common mistake when programming the arduino is the use of delay. For many applications you can get away with using delay but when programming with serial events, this is not the case. The problem exists in the buffer size for serial data. To prevent overflowing the buffer you must be reading the buffer regularly and the use of delay can prevent this.
For example in the uno the buffer is only 64 bytes. If you have a delay of 1 second in your code you cound fill that buffer over 16 times and potentially lose many important messages.
The best work around for this is to use microseconds and if statements Arduino has a great example built in, you can view it at [https://www.arduino.cc/en/Tutorial/BlinkWithoutDelay]


## Advanced PeachDuino
The advanced version of the api allows much lower level access to the data and works on a publish subscribe model.

###Basic Usage
This example subscribes to a message and changes a led when waiting for the correct height. It also attaches an interupt to count drips.
```c++
#include "messages.pb.h"
#include "peachduino.h"

#define WAITING_PIN 8

PeachDuino* peachDuino = new PeachDuino(Serial);  //Create a PeachyDunio API
unsigned long drips = 0;

void setup() {
  pinMode(WAITING_PIN, OUTPUT);
  peachDuino->addHandler(PRINTERSTATUS_TYPE, printerStatusHandler);  //Subscribe to the PrinterStatus message with the function printerStatusHandler
  attachInterrupt(digitalPinToInterrupt(2), incAndSendDrips, RISING);  //Interupt to catch drips)
  Serial.begin(9600);  //Start the serial connection the speed should match that set in the software
}

//The printer status handler is a method that will be called every time a PRINTERSTATUS message is recieved
void printerStatusHandler(void* newMessage) {
  PrinterStatus printerStatusMessage = *((PrinterStatus*)newMessage);   //casts the newMessage to the correct type
  if (printerStatusMessage.waitingForNextLayerHeight)
  {
    digitalWrite(WAITING_PIN, HIGH);
  } else {
    digitalWrite(WAITING_PIN, LOW);
  }
}

void incAndSendDrips() {
  drips++;
  DripRecorded dripRecordedMessage = {drips};   //Creates a DripRecorded message
  peachDuino->sendMessage(dripRecordedMessage);  //Sends the message immediatly
}

void loop(void) {
  peachDuino->process();  //You must call the process method regularly to ensure that all messages are processed.
}
```

###Subscribing
To handle incomming data, you can subscribe up to 8 message handlers to a number of the messages below.

Messages available and thier members:
* PrinterStatus
  * unsigned int targetHeightMicrometer       _(This is the requested height the software would like the printer to go to.)_
  * unsigned int currentHeightMicrometer      _(This is the current height the software believes the print to be at.)_
  * unsigned int waitingForNextLayerHeight    _(True if the printer is paused waiting for the height of the print to be correct.)_
  * Status status (enum short)                _(The status of the current print as defined below.)_
    * 0 - Unstarted
    * 1 - Printing
    * 2 - Waiting For Next Layer Height
    * 3 - Complete
* MoveToDripCount
  * unsigned int drips                        _(This is the number of drips the printer is expecting [Use as alternative to targetHeightMicrometer].)_
* SetDripCount
  * unsigned int drips                        _(This is tell the arduino what it's drip count should be.)

You subscribe to the messages you are intrested in based on a type:
* SETDRIPCOUNT_TYPE
* MOVETODRIPCOUNT_TYPE
* PRINTERSTATUS_TYPE

You need to provide a handler with the signature _void handlerMethodName(void* newMessage)_. You can then cast that to the expected type: _PrinterStatus printerStatusMessage = *((PrinterStatus*)newMessage)_

```c++
    void setup() {
      peachDuino->addHandler(PRINTERSTATUS_TYPE, printerStatusHandler);
    }

    void printerStatusHandler(void* newMessage) {
      PrinterStatus printerStatusMessage = *((PrinterStatus*)newMessage);
    }
```
###Publishing
When you want to tell the software informaiton you can publish an assorment of messages.
Messages available and thier members:
* DripRecorded
  * unsigned int drips  _(This tells the software how many drips have occured.)_
* SetCurrentHeight
  * unsigned int heightMicrometers _(This tells the software the current height.)_

```c++
    DripRecorded dripRecordedMessage = {drips};
    peachDuino->sendMessage(dripRecordedMessage);
```

###Helpers
* void process() _(This method process data in the serial buffer and should be called very regularly)_
* unsigned int success() _(Number of successfullly recieved messages.)_
* unsigned int fails() _(Number of failed messages. This may not include messages that did not arrive.)_
* unsigned long recieved() _(Number of bytes recieved.)_
* unsigned long sent() _(Number of bytes sent.)_