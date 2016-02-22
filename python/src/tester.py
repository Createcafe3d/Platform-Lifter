import serial
import time
from messages import SimpleMessage

class DoIt(object):
    HEADER = 0x42
    FOOTER = 0x41
    ESCAPE = 0x40

    def __init__(self, port, baud=192000):
        self.connection = serial.Serial(port, baud)
        time.sleep(3)

    def go(self):
        for i in range(100, 137):
            time.sleep(0.05)
            print(i)
            message = SimpleMessage(i, str(time.ctime()))
            mbytes = message.get_bytes()
            escaped = self._escape(mbytes)
            self.connection.write(escaped)
        self.connection.close()

    def _escape(self, in_bytes):
        print "-" * 30
        print ":".join("{}".format(ord(c)) for c in in_bytes)
        in_bytes = in_bytes.replace(chr(self.ESCAPE), chr(self.ESCAPE) + chr(self.ESCAPE))
        in_bytes = in_bytes.replace(chr(self.HEADER), chr(self.ESCAPE) + chr(self.HEADER))
        in_bytes = in_bytes.replace(chr(self.FOOTER), chr(self.ESCAPE) + chr(self.FOOTER))
        out_bytes = chr(self.HEADER) + in_bytes + chr(self.FOOTER)
        print ":".join("{}".format(ord(c)) for c in out_bytes)
        return out_bytes

if __name__ == '__main__':
    port = '/dev/ttyUSB0'
    DoIt(port).go()