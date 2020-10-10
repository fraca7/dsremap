#!/usr/bin/env python3

import logging

from PyQt5 import QtCore
import zeroconf


class NetworkDevice:
    def __init__(self, name, addr, port):
        self.name = name
        self.addr = addr
        self.port = port


class ZeroconfEnumerator(QtCore.QObject):
    _added = QtCore.pyqtSignal(object)
    _removed = QtCore.pyqtSignal(object)

    deviceAdded = QtCore.pyqtSignal(NetworkDevice)
    deviceRemoved = QtCore.pyqtSignal(NetworkDevice)

    logger = logging.getLogger('dsremap.zeroconf')

    def __init__(self):
        super().__init__()
        self._zeroconf = zeroconf.Zeroconf()
        self._browser = None
        self._items = []

        self._added.connect(self._onAdded)
        self._removed.connect(self._onRemoved)
        self._browser = zeroconf.ServiceBrowser(self._zeroconf, '_http._tcp.local.', self)

    def stop(self):
        self._zeroconf.close()

    def connect(self, target):
        for dev in self._items:
            target.onDeviceAdded(dev)
        self.deviceAdded.connect(target.onDeviceAdded)
        self.deviceRemoved.connect(target.onDeviceRemoved)

    def disconnect(self, target):
        self.deviceAdded.disconnect(target.onDeviceAdded)
        self.deviceRemoved.connect(target.onDeviceRemoved)
        for dev in self._items:
            target.onDeviceRemoved(dev)

    def _onAdded(self, info):
        if not info.name.startswith('DSRemap'):
            return
        dev = NetworkDevice(info.name, info.server, info.port)
        self._items.append(dev)
        self.deviceAdded.emit(dev)

    def _onRemoved(self, name):
        for dev in self._items:
            if dev.name == name:
                self.deviceRemoved.emit(dev)
                self._items.remove(dev)
                break

    def add_service(self, zc, type_, name): # pylint: disable=C0103
        info = zc.get_service_info(type_, name)
        self._added.emit(info)

    def remove_service(self, zc, type_, name): # pylint: disable=C0103,W0613
        self._removed.emit(name)

    def update_service(self, zc, type_, name): # pylint: disable=C0103
        pass
