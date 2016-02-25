#include "messages.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "Arduino.h"

#ifndef PeachDuino_h
#define PeachDuino_h

#define HEADER 0x00
#define FOOTER 0xFF
#define ESCAPE 0xFE

#define SIMPLETYPE 0x01;

class PeachDuino
{
  public:
    PeachDuino(HardwareSerial &hw_serial)
    {
      serial = &hw_serial;
    };

    bool process()
    {
      return readSerial();
    };

    void sendMessage()
    {
      _sendMessage();
    };

    int success()
    {
      return _success;
    };

    int fails()
    {
      return _fails;
    };

    int recieved = 0;
    int sent = 0;
    Simple message = Simple_init_zero;

  private:
    uint8_t inputBuffer[128];
    uint8_t outputBuffer[128];
    uint8_t readBuffer[64];
    uint8_t bufferPos = 1;
    bool escaped;
    int _fails = 0;
    int _success = 0;
    int available = 0;
    HardwareSerial* serial;

    void decode(uint8_t *buffer, size_t message_length) 
    {
      uint8_t TYPE_ID = buffer[0];
      pb_istream_t stream = pb_istream_from_buffer(buffer + 1, message_length - 1);
      bool status = pb_decode(&stream, Simple_fields, &message);
      if (!status)
      {
        _fails++;
      } else {
        _success++;
      }
    }

    void _sendMessage() {
      Simple message = {_success, "Success"};
      pb_ostream_t stream = pb_ostream_from_buffer(outputBuffer, sizeof(outputBuffer));
      bool status = pb_encode(&stream, Simple_fields, &message);
      uint8_t encodedBuffer[128];
      encodedBuffer[0] = HEADER;
      encodedBuffer[1] = SIMPLETYPE;
      int encodedBufferIndex = 2;
      for (int outputBufferIndex = 0; outputBufferIndex < stream.bytes_written; outputBufferIndex++){
        switch(outputBuffer[outputBufferIndex]){
          case ESCAPE:
            encodedBuffer[encodedBufferIndex] = ESCAPE;
            encodedBufferIndex++;
            break;
          case HEADER:
            encodedBuffer[encodedBufferIndex] = ESCAPE;
            encodedBufferIndex++;
            break;
          case FOOTER:
            encodedBuffer[encodedBufferIndex] = ESCAPE;
            encodedBufferIndex++;
            break;
        }
        encodedBuffer[encodedBufferIndex] = outputBuffer[outputBufferIndex];
        encodedBufferIndex++;
      }
      encodedBuffer[encodedBufferIndex] = FOOTER;
      sent = sent + encodedBufferIndex;
      serial->write(encodedBuffer, encodedBufferIndex + 1);
    }

    bool readSerial()
    {
      available = serial->available();
      if (available) {
        recieved = recieved + available;
        serial->readBytes(readBuffer, available);
        for (short i = 0; i < available; i++) {
          inputBuffer[bufferPos] = readBuffer[i];
          if (escaped) {
            bufferPos++;
            escaped = false;
          } else {
            switch(inputBuffer[bufferPos]) {
              case ESCAPE:
                escaped = true;
                break;
              case HEADER:
                bufferPos = 0;
                break;
              case FOOTER:
                decode(inputBuffer, bufferPos);
                break;
              default:
                bufferPos++;
                break;
            }
          }
        }
        return true;
      } else {
        return false;
      }
    }
};

#endif