#include "peachduino.h"

#ifndef SimplePeachDuino_h
#define SimplePeachDuino_h

class SimplePeachDuino: public Wrap
{
  public:
    SimplePeachDuino(HardwareSerial &hw_serial)
    {
      _peachDuino= new PeachDuino(hw_serial);
      _peachDuino->addHandler(PRINTERSTATUSTYPE, &this->printerStatusHandler);
    };

    bool process()
    {
      return _peachDuino->process();
    };

    unsigned int success()
    {
      return _peachDuino->success();
    };

    unsigned int fails()
    {
      return _peachDuino->fails();
    };

    int recieved()
    {
      return _peachDuino->recieved();
    };

    int sent()
    {
      return _peachDuino->sent();
    };

    void sendHeightMicrometer(unsigned int height)
    {
      SetCurrentHeight setCurrentHeightMessage = {height};
      _peachDuino->sendMessage(setCurrentHeightMessage);
    };

    void sendDripCount(unsigned int drips)
    {
      DripRecorded dripRecordedMessage = {drips};
      _peachDuino->sendMessage(dripRecordedMessage);
    };

    PeachDuino* _peachDuino;
    unsigned int targetHeightMicrometer = 0;
    unsigned int currentHeightMicrometer = 0;
    bool waitingForNextLayerHeight = true;
    short printStatus = 0;
    // 0 - Unstarted
    // 1 - Printing
    // 2 - Waiting For Next Layer Height
    // 3 - Complete

  private:
    void printerStatusHandler(void* newMessage) {
        PrinterStatus printerStatusMessage = *((PrinterStatus*)newMessage);
        targetHeightMicrometer = printerStatusMessage.targetHeightMicrometer;
        currentHeightMicrometer = printerStatusMessage.currentHeightMicrometer;
        waitingForNextLayerHeight = printerStatusMessage.waitingForNextLayerHeight;
        printStatus = 255;
      };
};

#endif