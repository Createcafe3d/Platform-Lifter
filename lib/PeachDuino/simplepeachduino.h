#include "peachduino.h"

#ifndef SimplePeachDuino_h
#define SimplePeachDuino_h

class SimplePeachDuino: public Wrap
{
  public:
    SimplePeachDuino(HardwareSerial &hw_serial)
    {
      _peachDuino= new PeachDuino(hw_serial);
      _peachDuino->addHandler(PRINTERSTATUS_TYPE, &Wrap::printerStatusHandler, this);
      _peachDuino->addHandler(MOVETODRIPCOUNT_TYPE, &Wrap::moveToDripCountHandler, this);
    };

    short process()
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

    unsigned long recieved()
    {
      return _peachDuino->recieved();
    };

    unsigned long sent()
    {
      return _peachDuino->sent();
    };

    void sendHeightMicrometer(unsigned long height)
    {
      SetCurrentHeight setCurrentHeightMessage = {height};
      _peachDuino->sendMessage(setCurrentHeightMessage);
    };

    void sendDripCount(unsigned long drips)
    {
      DripRecorded dripRecordedMessage = {drips};
      _peachDuino->sendMessage(dripRecordedMessage);
    };

    unsigned long targetHeightMicrometer = 0;
    unsigned long currentHeightMicrometer = 0;
    bool waitingForNextLayerHeight = true;
    short printStatus = 0;
    // 0 - Unstarted
    // 1 - Printing
    // 2 - Waiting For Next Layer Height
    // 3 - Complete
    unsigned long targetDripCount = 0;

  private:
    PeachDuino* _peachDuino;
    virtual void printerStatusHandler(void* newMessage) {
        PrinterStatus printerStatusMessage = *((PrinterStatus*)newMessage);
        targetHeightMicrometer = printerStatusMessage.targetHeightMicrometer;
        currentHeightMicrometer = printerStatusMessage.currentHeightMicrometer;
        waitingForNextLayerHeight = printerStatusMessage.waitingForNextLayerHeight;
        printStatus = printerStatusMessage.status;
      };
    virtual void moveToDripCountHandler(void* newMessage) {
        MoveToDripCount moveToDripCountMessage = *((MoveToDripCount*)newMessage);
        targetDripCount = moveToDripCountMessage.drips;
      };
};

#endif