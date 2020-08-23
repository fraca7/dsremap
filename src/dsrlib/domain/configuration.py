#!/usr/bin/env python3

import io
import struct
import uuid

from PyQt5 import QtCore

from .listmodel import ListModel


class Configuration(QtCore.QObject):
    changed = QtCore.pyqtSignal()

    def __init__(self, uid=None):
        super().__init__()
        self._name = _('New configuration')
        self._thumbnail = None
        self._actions = ListModel()
        self._enabled = False
        self._uuid = uid or uuid.uuid1().hex

    def uuid(self):
        return self._uuid

    def name(self):
        return self._name

    def setName(self, name):
        if name != self._name:
            self._name = name
            self.changed.emit()

    def enabled(self):
        return self._enabled

    def setEnabled(self, enabled):
        if enabled != self._enabled:
            self._enabled = enabled
            self.changed.emit()

    def thumbnail(self):
        return self._thumbnail

    def setThumbnail(self, filename):
        self._thumbnail = filename
        self.changed.emit()

    def actions(self):
        return self._actions

    def addAction(self, action, index=None):
        action.changed.connect(self.changed)
        self._actions.addItem(action, index=index)
        self.changed.emit()

    def removeAction(self, action):
        action.changed.disconnect(self.changed)
        self._actions.removeItem(action)
        self.changed.emit()

    def bytecodeSize(self):
        total = 2 # first 2 bytes: total size of configuration
        for action in self.actions():
            total += action.size()
        return total

    def bytecode(self):
        bytecode = io.BytesIO()
        for action in self._actions:
            bytecode.write(action.bytecode())
        return struct.pack('<H', bytecode.tell()) + bytecode.getvalue()
