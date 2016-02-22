from messages_pb2 import Simple


class ProtoBuffableMessage(object):
    TYPE_ID = 0

    def get_bytes(self):
        raise NotImplementedError()

    @classmethod
    def from_bytes(cls, proto_bytes):
        raise NotImplementedError()


class SimpleMessage(ProtoBuffableMessage):
    TYPE_ID = 1

    def __init__(self, lucky_number, message):
        self._lucky_number = lucky_number
        self._message = message

    @property
    def lucky_number(self):
        return self._lucky_number

    @property
    def message(self):
        return self._message

    def get_bytes(self):
        encoded = Simple()
        encoded.lucky_number = self._lucky_number
        encoded.message = self._message

        if encoded.IsInitialized():
            return encoded.SerializeToString()
        else:
            raise Exception("Protobuf Message encoding incomplete")

    @classmethod
    def from_bytes(cls, proto_bytes):
        decoded = Simple()
        decoded.ParseFromString(proto_bytes)
        return cls(decoded.lucky_number, decoded.message)

    def __eq__(self, other):
        if (self.__class__ == other.__class__ and
                self._lucky_number == other._lucky_number and
                self._message == other._message
                ):
            return True
        else:
            return False

    def __repr__(self):
        return "Lucky: {} Message: {}".format(self._lucky_number, self._message)
