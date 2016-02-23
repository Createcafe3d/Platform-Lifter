import serial
import threading
import time

from messages import ProtoBuffableMessage

class Communicator(object):
    def send(self, message):
        raise NotImplementedError()

    def register_handler(self, message_type, handler):
        raise NotImplementedError()


class ArduinoCommunicator(Communicator, threading.Thread):
    HEADER = 0x00
    FOOTER = 0xFF
    ESCAPE = 0xFE

    def __init__(self, port, baud=14400):
        self.port = port
        self.baud = baud
        self._running = False
        self._run_lock = threading.Lock()
        self._data_lock = threading.Lock()
        self.queue = []
        threading.Thread.__init__(self)

    def run(self):
        with self._run_lock:
            self._connection = serial.Serial(self.port, self.baud)
            time.sleep(3)
            self._running = True
            while self._running:
                if self.queue:
                    with self._data_lock:
                        write_bytes = "".join(self.queue)
                        self.queue = []
                    print ":".join("{}".format(ord(c)) for c in write_bytes)
                    self._connection.write(write_bytes)
                    print("Wrote {} bytes".format(len(write_bytes)))
            self._connection.close()

    def _encode(self, message):
        raw_bytes = chr(message.TYPE_ID) + message.get_bytes()
        escaped_bytes = raw_bytes.replace(chr(self.ESCAPE), chr(self.ESCAPE) + chr(self.ESCAPE))
        escaped_bytes = escaped_bytes.replace(chr(self.HEADER), chr(self.ESCAPE) + chr(self.HEADER))
        escaped_bytes = escaped_bytes.replace(chr(self.FOOTER), chr(self.ESCAPE) + chr(self.FOOTER))
        return chr(self.HEADER) + escaped_bytes + chr(self.FOOTER)

    def _decode(self, message):
        pass

    def close(self):
        self._running = False
        self._run_lock.acquire()
        self._run_lock.release()

    def send(self, message):
        message_bytes = self._encode(message)
        with self._data_lock:
            self.queue.append(message_bytes)

    def register_handler(self, message_type, handler):
        if not issubclass(message_type, ProtoBuffableMessage):
            raise Exception("ProtoBuffableMessage required for message type")
        with self._handler_lock:
            if message_type in self._handlers:
                self._handlers[message_type].append(handler)
            else:
                self._handlers[message_type] = [handler]
