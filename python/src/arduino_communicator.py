import serial
import threading
import time

from messages import ProtoBuffableMessage, SimpleMessage

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

        self.input_buffer = ""
        self.current_message = ""
        self.in_message = False
        self.input_escaped = False
        self.is_identifier = False
        self.current_message_identifier = ""
        threading.Thread.__init__(self)

    def run(self):
        with self._run_lock:
            self._connection = serial.Serial(self.port, self.baud, timeout=0.01)
            time.sleep(3)
            self._running = True
            while self._running:
                if self.queue:
                    with self._data_lock:
                        write_bytes = "".join(self.queue)
                        self.queue = []
                    # print ":".join("{}".format(ord(c)) for c in write_bytes)
                    self._connection.write(write_bytes)
                    print("Wrote {} bytes".format(len(write_bytes)))
                self.input_buffer = self._connection.read(16)
                self._process_data()
            self._connection.close()

    def _process_data(self):
        if len(self.input_buffer):
            print("Read {} bytes".format(len(self.input_buffer)))
        for byte in self.input_buffer:
            if self.is_identifier:
                self.is_identifier = False
                self.current_message_identifier = ord(byte)
            elif self.input_escaped:
                self.input_escaped = False
                self.current_message += byte
            else:
                if ord(byte) == self.ESCAPE:
                    self.input_escaped = True
                elif ord(byte) == self.HEADER:
                    self.current_message = ""
                    self.is_identifier = True
                    self.in_message = True
                elif ord(byte) == self.FOOTER:
                    if self.in_message:
                        try:
                            self._decode(self.current_message_identifier, self.current_message)
                        except:
                            print("Failed to Decode")
                    self.in_message = False
                else:
                    self.current_message += byte
        self.input_buffer = ""


    def _encode(self, message):
        raw_bytes = chr(message.TYPE_ID) + message.get_bytes()
        escaped_bytes = raw_bytes.replace(chr(self.ESCAPE), chr(self.ESCAPE) + chr(self.ESCAPE))
        escaped_bytes = escaped_bytes.replace(chr(self.HEADER), chr(self.ESCAPE) + chr(self.HEADER))
        escaped_bytes = escaped_bytes.replace(chr(self.FOOTER), chr(self.ESCAPE) + chr(self.FOOTER))
        return chr(self.HEADER) + escaped_bytes + chr(self.FOOTER)

    def _decode(self, identifier, message):
        m = SimpleMessage.from_bytes(message)
        print("\n{}".format(str(m)))

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
