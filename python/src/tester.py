import serial
import time
import threading
from messages import SimpleMessage
from arduino_communicator import ArduinoCommunicator

class DoIt(object):
    HEADER = 0x00
    FOOTER = 0xFF
    ESCAPE = 0xFE


    def __init__(self, port, baud=14400):
        self.connection = ArduinoCommunicator(port, baud)
        self.connection.start()
        time.sleep(5)

    def go(self):

        for i in range(888, 899):
            time.sleep(0.05)
            print(i)
            message = SimpleMessage(i, str(time.ctime()))
            self.connection.send(message)
        self.connection.close()

    # def _encode(self, in_bytes):
    #     print "-" * 30
    #     print ":".join("{}".format(ord(c)) for c in in_bytes)
    #     in_bytes = in_bytes.replace(chr(self.ESCAPE), chr(self.ESCAPE) + chr(self.ESCAPE))
    #     in_bytes = in_bytes.replace(chr(self.HEADER), chr(self.ESCAPE) + chr(self.HEADER))
    #     in_bytes = in_bytes.replace(chr(self.FOOTER), chr(self.ESCAPE) + chr(self.FOOTER))
    #     out_bytes = chr(self.HEADER) + in_bytes + chr(self.FOOTER)
    #     print ":".join("{}".format(ord(c)) for c in out_bytes)
    #     return out_bytes

if __name__ == '__main__':
    port = '/dev/ttyUSB2'
    DoIt(port).go()