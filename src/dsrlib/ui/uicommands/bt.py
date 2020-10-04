#!/usr/bin/env python3

from PyQt5 import QtWidgets

from dsrlib.ui.mixins import MainWindowMixin
from dsrlib.ui.pairer import PairingWizard

from .base import UICommand


class PairUICommand(UICommand):
    def __init__(self, parent, *, device, enumerator, **kwargs):
        super().__init__(parent, text=device.info.name, tip=_('Pair a DualShock 4 and a PS4 to this DSRemap-enabled device'), **kwargs)
        self._device = device
        self._enumerator = enumerator

    def do(self):
        wizard = PairingWizard(self.mainWindow(), self._device, self._enumerator)
        wizard.exec_()


class PairMenu(MainWindowMixin, QtWidgets.QMenu):
    def __init__(self, parent, *, enumerator, hidenum, **kwargs):
        super().__init__(_('Pair'), parent, **kwargs)
        enumerator.deviceAdded.connect(self._deviceAdded)
        enumerator.deviceRemoved.connect(self._deviceRemoved)
        self._hidenum = hidenum
        self._devices = []

        self._dummy = self.addAction(_('No devices detected'))
        self._dummy.setEnabled(False)

    def _deviceAdded(self, dev):
        if not self._devices:
            self.removeAction(self._dummy)

        self._devices.append(dev)
        uicmd = PairUICommand(self, device=dev, enumerator=self._hidenum, mainWindow=self.mainWindow())
        self.addAction(uicmd)

    def _deviceRemoved(self, dev):
        for index, other in enumerate(self._devices):
            if other.info == dev.info:
                break
        else:
            return

        del self._devices[index] # pylint: disable=W0631
        for idx, action in enumerate(self.actions()):
            if idx == index: # pylint: disable=W0631
                self.removeAction(action)
                break

        if not self._devices:
            self.addAction(self._dummy)
