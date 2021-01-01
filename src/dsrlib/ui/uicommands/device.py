#!/usr/bin/env python3

from PyQt5 import QtWidgets

from dsrlib.meta import Meta
from dsrlib.ui.mixins import MainWindowMixin
from dsrlib.domain.mixins import WorkspaceMixin
from dsrlib.domain.device import DeviceVisitor

from .misc import UpdateHexUICommand, SetupSDCardUICommand
from .hid import UploadConfigurationsUICommand
from .bt import NetworkDeviceMenu


class DeviceMenuBase(MainWindowMixin, WorkspaceMixin, QtWidgets.QMenu):
    def __init__(self, text, parent, *, enumerator, **kwargs):
        super().__init__(text, parent, **kwargs)
        self._enumerator = enumerator
        self._dummy = None
        self._devices = []

    def start(self):
        self._dummy = self.addAction(_('No device detected'))
        self._dummy.setEnabled(False)
        self._enumerator.connect(self)

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
        for index, (other, action) in enumerate(self._devices):
            if device == other:
                break
        else:
            return

        del self._devices[index] # pylint: disable=W0631
        self.removeAction(action) # pylint: disable=W0631
        if not self._devices:
            self.addAction(self._dummy)

    def onNetworkDeviceAdded(self, device):
        raise NotImplementedError

    def onArduinoAdded(self, device):
        raise NotImplementedError

    def addDeviceAction(self, device, action):
        if not self._devices:
            self.removeAction(self._dummy)
        self._devices.append((device, action))
        self.addAction(action)


class DeviceMenu(DeviceMenuBase):
    def __init__(self, parent, *args, **kwargs):
        super().__init__(_('Devices'), parent, *args, **kwargs)

        self.addAction(UpdateHexUICommand(self, mainWindow=self.mainWindow()))
        self.addAction(SetupSDCardUICommand(self, mainWindow=self.mainWindow()))
        self.addSeparator()
        self.start()

    def onNetworkDeviceAdded(self, device):
        self.addDeviceAction(device, NetworkDeviceMenu(self, device=device, enumerator=self._enumerator, mainWindow=self.mainWindow(), workspace=self.workspace()))

    def onArduinoAdded(self, device):
        pass


class UploadMenu(DeviceMenuBase):
    def __init__(self, parent, *args, container, **kwargs):
        super().__init__(_('Upload'), parent, *args, **kwargs)
        self._container = container
        self.start()

    def onNetworkDeviceAdded(self, device):
        self.addDeviceAction(device, UploadConfigurationsUICommand(self, device=device, container=self._container, mainWindow=self.mainWindow(), workspace=self.workspace()))

    def onArduinoAdded(self, device):
        if Meta.firmwareVersion() != device.fwVersion:
            QtWidgets.QMessageBox.warning(self.mainWindow(),
                                          _('Firmware version mismatch'),
                                          _('A {appname}-enabled device was plugged, but the firmware version v{version1} does not match the one supported by this application (v{version2}). Please update the firmware using the Upload menu first, then unplug and replug the device.').format(appname=Meta.appName(), version1=str(device.fwVersion), version2=str(Meta.firmwareVersion())))
            return

        self.addDeviceAction(device, UploadConfigurationsUICommand(self, device=device, container=self._container, mainWindow=self.mainWindow(), workspace=self.workspace()))
