#!/usr/bin/env python3

import json
import logging
import functools

from PyQt5 import QtCore, QtNetwork
import zeroconf


class ZeroconfItem:
    def __init__(self, info, btinfo):
        self.info = info
        self.btinfo = btinfo


class ZeroconfEnumerator(QtCore.QObject):
    _added = QtCore.pyqtSignal(object)
    _removed = QtCore.pyqtSignal(object)

    deviceAdded = QtCore.pyqtSignal(ZeroconfItem)
    deviceRemoved = QtCore.pyqtSignal(ZeroconfItem)

    logger = logging.getLogger('dsremap.zeroconf')

    def __init__(self, manager):
        super().__init__()
        self._zeroconf = zeroconf.Zeroconf()
        self._browser = None
        self._pending = []
        self._items = []
        self._manager = manager

        self._added.connect(self._onAdded)
        self._removed.connect(self._onRemoved)

    def start(self):
        self._browser = zeroconf.ServiceBrowser(self._zeroconf, '_http._tcp.local.', self)
        self._manager.finished.connect(self._onResponse)

    def _onAdded(self, info):
        if not info.name.startswith('DSRemap'):
            return

        reply = self._manager.get(QtNetwork.QNetworkRequest(QtCore.QUrl('http://%s:%d/info' % (info.server, info.port))))
        self._pending.append((info, reply))

    def _onResponse(self, reply):
        for idx, (info, other) in enumerate(self._pending):
            if other == reply:
                self._pending.pop(idx)
                status = reply.attribute(QtNetwork.QNetworkRequest.HttpStatusCodeAttribute)
                if status != 200:
                    self.logger.warning('Cannot get host info: %s', status)
                    return
                btinfo = json.loads(bytes(reply.readAll()).decode('utf-8'))

                item = ZeroconfItem(info, btinfo)
                self._items.append(item)
                self.deviceAdded.emit(item)
                break

    def _onRemoved(self, info):
        for idx, (other, reply) in enumerate(self._pending):
            if other == info:
                self._pending.pop(idx)

                for item in self._items:
                    if item.info == info:
                        self._items.remove(item)
                        self.deviceRemoved.emit(item)
                        break
                break

    def shutdown(self):
        self._manager.finished.disconnect(self._onResponse)
        self._zeroconf.close()

    def add_service(self, zc, type_, name):
        info = zc.get_service_info(type_, name)
        self._added.emit(info)

    def remove_service(self, zc, type_, name):
        info = zc.get_service_info(type_, name)
        self._removed.emit(info)

    def update_service(self, zc, type_, name):
        pass
