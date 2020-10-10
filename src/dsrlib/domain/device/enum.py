#!/usr/bin/env python3

from .network import ZeroconfEnumerator
from .local import HIDEnumerator


class DeviceEnumerator:
    def __init__(self):
        self._network = ZeroconfEnumerator()
        self._local = HIDEnumerator()
        self._local.start()

    def stop(self):
        self._network.stop()
        self._local.stop()

    def connect(self, target):
        self._network.connect(target)
        self._local.connect(target)

    def disconnect(self, target):
        self._network.disconnect(target)
        self._local.disconnect(target)
