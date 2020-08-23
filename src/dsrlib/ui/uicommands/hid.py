#!/usr/bin/env python3

from PyQt5 import QtWidgets

from dsrlib.meta import Meta
from dsrlib.domain.mixins import WorkspaceMixin
from dsrlib.ui.mixins import MainWindowMixin
from dsrlib.ui.configupload import ConfigurationUploader

from .base import UICommand


class UploadConfigurationsUICommand(WorkspaceMixin, UICommand):
    def __init__(self, parent, *, device, **kwargs):
        super().__init__(parent, text=device.name, tip=_('Send enabled configurations to EEPROM of device {name}').format(name=device.name), **kwargs)
        self._device = device

        self.add_signal_check(self.workspace().configurations().dataChanged)
        self.add_signal_check(self.workspace().configurations().rowsInserted)
        self.add_signal_check(self.workspace().configurations().rowsRemoved)

    def should_be_enabled(self):
        return super().should_be_enabled() and self.workspace().bytecodeSize() <= Meta.maxBytecodeSize()

    def do(self):
        uploader = ConfigurationUploader(self.mainWindow(), device=self._device, workspace=self.workspace())
        uploader.exec_()


class UploadMenu(MainWindowMixin, WorkspaceMixin, QtWidgets.QMenu):
    def __init__(self, parent, *, enumerator, **kwargs):
        super().__init__(_('Upload configurations to'), parent, **kwargs)
        enumerator.deviceAdded.connect(self._deviceAdded)
        enumerator.deviceRemoved.connect(self._deviceRemoved)
        self._devices = []

        self._dummy = self.addAction(_('No devices detected'))
        self._dummy.setEnabled(False)

    def _deviceAdded(self, dev):
        if Meta.firmwareVersion() != dev.fwVersion:
            QtWidgets.QMessageBox.warning(self.mainWindow(),
                                          _('Firmware version mismatch'),
                                          _('A {appname}-enabled device was plugged, but the firmware version v{version1} does not match the one supported by this application (v{version2}). Please update the firmware using the Upload menu first, then unplug and replug the device.').format(appname=Meta.appName(), version1=str(dev.fwVersion), version2=str(Meta.firmwareVersion())))
            return

        if not self._devices:
            self.removeAction(self._dummy)

        self._devices.append(dev)
        uicmd = UploadConfigurationsUICommand(self, device=dev, mainWindow=self.mainWindow(), workspace=self.workspace())
        self.addAction(uicmd)

    def _deviceRemoved(self, dev):
        for index, other in enumerate(self._devices):
            if other.path == dev.path:
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
