#!/usr/bin/env python3

from PyQt5 import QtCore, QtWidgets, QtNetwork

from dsrlib.domain.mixins import WorkspaceMixin
from dsrlib.ui.wizard import PairingWizard
from dsrlib.ui.mixins import MainWindowMixin

from .base import UICommand


class PairUICommand(UICommand):
    def __init__(self, parent, *, device, enumerator, **kwargs):
        super().__init__(parent, text=_('Pair with a Dualshock'), tip=_('Pair a DualShock 4 to this DSRemap-enabled device'), **kwargs)
        self._device = device
        self._enumerator = enumerator

    def do(self):
        wizard = PairingWizard(self.mainWindow(), device=self._device, enumerator=self._enumerator, mainWindow=self.mainWindow())
        wizard.exec_()


class RebootDeviceUICommand(UICommand):
    def __init__(self, parent, *, device, **kwargs):
        super().__init__(parent, text=_('Reboot'), **kwargs)
        self._device = device

    def do(self):
        url = QtCore.QUrl('http://%s:%d/reboot' % (self._device.addr, self._device.port))
        self.mainWindow().manager().get(QtNetwork.QNetworkRequest(url))


class HaltDeviceUICommand(UICommand):
    def __init__(self, parent, *, device, **kwargs):
        super().__init__(parent, text=_('Halt'), **kwargs)
        self._device = device

    def do(self):
        url = QtCore.QUrl('http://%s:%d/halt' % (self._device.addr, self._device.port))
        self.mainWindow().manager().get(QtNetwork.QNetworkRequest(url))


class NetworkDeviceMenu(MainWindowMixin, WorkspaceMixin, QtWidgets.QAction):
    def __init__(self, parent, *, device, enumerator, **kwargs):
        super().__init__(device.name, parent, **kwargs)

        menu = QtWidgets.QMenu(self.mainWindow())
        menu.addAction(PairUICommand(self, mainWindow=self.mainWindow(), device=device, enumerator=enumerator))
        menu.addAction(RebootDeviceUICommand(self, mainWindow=self.mainWindow(), device=device))
        menu.addAction(HaltDeviceUICommand(self, mainWindow=self.mainWindow(), device=device))
        self.setMenu(menu)
