import unittest
import sys
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from messages import ArduinoCommunicator


class ArduinoCommunicatorTests(unittest.TestCase):
    def test_send(self):
        pass

if __name__ == '__main__':
    unittest.main()