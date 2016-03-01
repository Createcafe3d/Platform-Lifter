import serial
import time
import threading
from messages import PrinterStatusMessage
from arduino_communicator import ArduinoCommunicator


class DoIt(object):
    HEADER = 0x00
    FOOTER = 0xFF
    ESCAPE = 0xFE


    def __init__(self, port, baud=9600):
        self.connection = ArduinoCommunicator(port, baud)
        self.connection.start()
        time.sleep(3)

    def go(self):
        for i in range(0, 20):
            print(i)
            message = PrinterStatusMessage(i, i+1, i % 2 == 0, i % 4)
            self.connection.send(message)
            time.sleep(0.5)
        self.connection.close()

if __name__ == '__main__':
    port = '/dev/ttyUSB0'
    DoIt(port).go()