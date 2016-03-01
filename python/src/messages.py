from messages_pb2 import PrinterStatus


class ProtoBuffableMessage(object):
    TYPE_ID = 0

    def get_bytes(self):
        raise NotImplementedError()

    @classmethod
    def from_bytes(cls, proto_bytes):
        raise NotImplementedError()


class PrinterStatusMessage(ProtoBuffableMessage):
    TYPE_ID = 51

    def __init__(self, targetHeightMicrometer, currentHeightMicrometer, waitingForNextLayerHeight, status):
        self._target_height_micrometer = targetHeightMicrometer
        self._current_height_micrometer = currentHeightMicrometer
        self._waiting_for_next_layer_height = waitingForNextLayerHeight
        self._status = status

    @property
    def target_height_micrometer():
        return self._target_height_micrometer

    @property
    def current_height_micrometer():
        return self._current_height_micrometer

    @property
    def waiting_for_next_layer_height():
        return self._waiting_for_next_layer_height

    @property
    def status():
        return self._status


    def get_bytes(self):
        encoded = PrinterStatus()
        encoded.targetHeightMicrometer = self._target_height_micrometer
        encoded.currentHeightMicrometer = self._current_height_micrometer
        encoded.waitingForNextLayerHeight = self._waiting_for_next_layer_height
        encoded.status = self._status

        if encoded.IsInitialized():
            return encoded.SerializeToString()
        else:
            raise Exception("Protobuf Message encoding incomplete")

    @classmethod
    def from_bytes(cls, proto_bytes):
        decoded = PrinterStatus()
        decoded.ParseFromString(proto_bytes)
        return cls(
            decoded.targetHeightMicrometer,
            decoded.currentHeightMicrometer,
            decoded.waitingForNextLayerHeight,
            decoded.status,
            )

    def __eq__(self, other):
        if (self.__class__ == other.__class__ and
                self._target_height_micrometer == other._target_height_micrometer and
                self._current_height_micrometer == other._current_height_micrometer and
                self._waiting_for_next_layer_height == other._waiting_for_next_layer_height and
                self._status == other._status
                ):
            return True
        else:
            return False

    def __repr__(self):
        return ("Target Height            : {}\n"
               "Current Height           : {}\n"
               "Waiting for next height  : {}\n"
               "Status                   : {}").format(
                self._target_height_micrometer,
                self._current_height_micrometer,
                self._waiting_for_next_layer_height,
                self._status)
