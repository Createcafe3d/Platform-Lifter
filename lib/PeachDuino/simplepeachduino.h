class SimplePeachDuino
{
  public:
    PeachDuino(HardwareSerial &hw_serial)
    {
      serial = &hw_serial;
      ##MAGIC
    };

    bool process()
    {
      return readSerial();
    };

    unsigned int success()
    {
      return _success;
    };

    unsigned int fails()
    {
      return _fails;
    };

   


    #--------------------------

    void sendHeightMicrometer(unsigned int height)
    {

    };

    void sendDripCount(unsigned int drips)
    {

    };

    unsigned int targetHeightMicrometer = 0;
    unsigned int currentHeightMicrometer = 0;
    bool waitingForNextLayerHeight = true;
    short printStatus = 0;
    // 0 - Unstarted
    // 1 - Printing
    // 2 - Waiting For Next Layer Height
    // 3 - Complete


  private:
    void setTargetHeightMicrometer(magic)
    {
      targetHeightMicrometer = magic.height
    };

    void setWaitingForLayer(magic)
    {
      
    };

    void setCurrentHeightMicrometer(magic)
    {
      targetHeightMicrometer = magic.height
    };

    void setHeightMicrometer()
    {

    };
}