#!/usr/bin/env python3

from dsrlib.meta import Meta
from dsrlib.domain.mixins import WorkspaceMixin
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
