#include "messages.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "Arduino.h"

#ifndef PeachDuino_h
#define PeachDuino_h

#define HEADER 0x00
#define FOOTER 0xFF
#define ESCAPE 0xFE

#define PRINTERSTATUSTYPE 51;

struct Handler {
  uint8_t typeId;
  void (*handler)(void* message);
};

struct Handler g_handlers[32];
unsigned int g_handler_count = 0;


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

    void sendMessage(DripRecorded dripRecordedMessage)
    {
      pb_ostream_t stream = pb_ostream_from_buffer(outputBuffer, sizeof(outputBuffer));
      bool status = pb_encode(&stream, DripRecorded_fields, &dripRecordedMessage);
      _sendBytes(3, stream);
    };

    int success()
    {
      return _success;
    };

    int fails()
    {
      return _fails;
    };

    void addHandler(unsigned int typeId, void (*handler)(void* message)) {
      g_handlers[g_handler_count].typeId = typeId;
      g_handlers[g_handler_count].handler = handler;
      g_handler_count++;
    }

    unsigned long recieved = 0;
    unsigned long sent = 0;

  private:
    uint8_t inputBuffer[64];

    uint8_t outputBuffer[64];
    uint8_t readBuffer[64];
    uint8_t bufferPos = 1;
    bool escaped;
    int _fails = 0;
    int _success = 0;
    int available = 0;
    HardwareSerial* serial;

    void decode(uint8_t *buffer, size_t message_length) 
    {
      uint8_t typeId = buffer[0];
      bool status;
      void *message;
      pb_istream_t stream = pb_istream_from_buffer(buffer + 1, message_length - 1);
      switch(typeId){
        case 51:
          PrinterStatus printerStatusMessage = PrinterStatus_init_zero;
          message = &printerStatusMessage;
          status = pb_decode(&stream, PrinterStatus_fields, message);
          break;
      }
      for(int i=0; i < g_handler_count; i++) {
        if (g_handlers[i].typeId == typeId) {
          g_handlers[i].handler(message);
        }
      }

      if (!status)
      {
        _fails++;
      } else {
        _success++;
      }
    }

    void _sendBytes(byte id, pb_ostream_t stream) {
      uint8_t encodedBuffer[64];
      encodedBuffer[0] = HEADER;
      encodedBuffer[1] = id;
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