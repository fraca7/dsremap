#!/usr/bin/env python3

from PyQt5 import QtWidgets

from dsrlib.meta import Meta
from dsrlib.ui.mixins import MainWindowMixin
from dsrlib.domain.mixins import WorkspaceMixin
from dsrlib.domain.device import DeviceVisitor

from .misc import UpdateHexUICommand
from .hid import UploadConfigurationsUICommand
from .bt import PairUICommand


class DeviceMenu(MainWindowMixin, WorkspaceMixin, QtWidgets.QMenu):
    def __init__(self, parent, *, enumerator, **kwargs):
        super().__init__(_('Devices'), parent, **kwargs)
        self._enumerator = enumerator

        self.addAction(UpdateHexUICommand(self, mainWindow=self.mainWindow()))
        self.addSeparator()
        self._dummy = self.addAction(_('No device detected'))
        self._dummy.setEnabled(False)

        self._devices = []
        enumerator.connect(self)

    def onDeviceAdded(self, device):
        class Visitor(DeviceVisitor):
            def __init__(self, listener):
                self.listener = listener

            def acceptNetworkDevice(self, dev):
                self.listener.onNetworkDeviceAdded(dev)

            def acceptArduino(self, dev):
                self.listener.onArduinoAdded(dev)

            def acceptDualshock(self, dev):
                pass

        Visitor(self).visit(device)

    def onDeviceRemoved(self, device):
        for index, other in enumerate(self._devices):
            if device == other:
                break
        else:
            return

        del self._devices[index] # pylint: disable=W0631
        for idx, action in enumerate(self.actions()[2:]):
            if idx == index: # pylint: disable=W0631
                self.removeAction(action)
                break

        if not self._devices:
            self.addAction(self._dummy)

    def onNetworkDeviceAdded(self, device):
        self._addAction(device, PairUICommand(self, device=device, enumerator=self._enumerator, mainWindow=self.mainWindow()))

    def onArduinoAdded(self, device):
        if Meta.firmwareVersion() != device.fwVersion:
            QtWidgets.QMessageBox.warning(self.mainWindow(),
                                          _('Firmware version mismatch'),
                                          _('A {appname}-enabled device was plugged, but the firmware version v{version1} does not match the one supported by this application (v{version2}). Please update the firmware using the Upload menu first, then unplug and replug the device.').format(appname=Meta.appName(), version1=str(device.fwVersion), version2=str(Meta.firmwareVersion())))
            return

        self._addAction(device, UploadConfigurationsUICommand(self, device=device, mainWindow=self.mainWindow(), workspace=self.workspace()))

    def _addAction(self, device, action):
        if not self._devices:
            self.removeAction(self._dummy)
        self._devices.append(device)
        self.addAction(action)
