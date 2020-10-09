#!/usr/bin/env python3

import logging

from PyQt5 import QtCore
import zeroconf


class ZeroconfItem:
    def __init__(self, info):
        self.info = info


class ZeroconfEnumerator(QtCore.QObject):
    _added = QtCore.pyqtSignal(object)
    _removed = QtCore.pyqtSignal(object)

    deviceAdded = QtCore.pyqtSignal(ZeroconfItem)
    deviceRemoved = QtCore.pyqtSignal(ZeroconfItem)

    logger = logging.getLogger('dsremap.zeroconf')

    def __init__(self):
        super().__init__()
        self._zeroconf = zeroconf.Zeroconf()
        self._browser = None
        self._items = []

        self._added.connect(self._onAdded)
        self._removed.connect(self._onRemoved)

    def start(self):
        self._browser = zeroconf.ServiceBrowser(self._zeroconf, '_http._tcp.local.', self)

    def _onAdded(self, info):
        if not info.name.startswith('DSRemap'):
            return
        item = ZeroconfItem(info)
        self._items.append(item)
        self.deviceAdded.emit(item)

    def _onRemoved(self, info):
        for item in self._items:
            if item.info == info:
                self.deviceRemoved.emit(item)
                self._items.remove(item)
                break

    def shutdown(self):
        self._zeroconf.close()

    def add_service(self, zc, type_, name): # pylint: disable=C0103
        info = zc.get_service_info(type_, name)
        self._added.emit(info)

    def remove_service(self, zc, type_, name): # pylint: disable=C0103
        info = zc.get_service_info(type_, name)
        self._removed.emit(info)

    def update_service(self, zc, type_, name): # pylint: disable=C0103
        pass
