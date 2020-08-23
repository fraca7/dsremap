#!/usr/bin/env python3

import struct

from PyQt5 import QtCore

from dsrlib.compiler.bcgen import generateBytecode


class Action(QtCore.QObject):
    changed = QtCore.pyqtSignal()

    def __init__(self):
        super().__init__()
        self._bytecode = None
        self.notifyChanged()

    def notifyChanged(self):
        _, bytecode = generateBytecode(self.source())
        # Header: action size
        self._bytecode = struct.pack('<H', bytecode.tell()) + bytecode.getvalue()
        self.changed.emit()

    def bytecode(self):
        return self._bytecode

    def size(self):
        return len(self._bytecode)

    def label(self):
        return self.labelFormat().format(**self.labelValues())

    def labelFormat(self):
        raise NotImplementedError

    def labelValues(self):
        raise NotImplementedError

    def source(self):
        raise NotImplementedError


class ActionVisitor:
    def visit(self, action):
        return getattr(self, '_accept%s' % action.__class__.__name__)(action)

    def _acceptInvertPadAxisAction(self, action):
        raise NotImplementedError

    def _acceptSwapAxisAction(self, action):
        raise NotImplementedError

    def _acceptGyroAction(self, action):
        raise NotImplementedError

    def _acceptCustomAction(self, action):
        raise NotImplementedError
