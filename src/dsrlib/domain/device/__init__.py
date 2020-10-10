#!/usr/bin/env python3

from .local import Dualshock, Arduino
from .network import NetworkDevice
from .enum import DeviceEnumerator


class DeviceVisitor:
    def visit(self, dev):
        getattr(self, 'accept%s' % dev.__class__.__name__)(dev)

    def acceptNetworkDevice(self, dev):
        raise NotImplementedError

    def acceptDualshock(self, dev):
        raise NotImplementedError

    def acceptArduino(self, dev):
        raise NotImplementedError
