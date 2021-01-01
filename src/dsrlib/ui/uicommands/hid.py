#!/usr/bin/env python3

from pyqtcmd import NeedsSelectionUICommandMixin

from dsrlib.domain.mixins import WorkspaceMixin
from dsrlib.domain.device import NetworkDevice
from dsrlib.ui.configupload import ConfigurationHIDUploader, ConfigurationNetworkUploader

from .base import UICommand


class UploadConfigurationsUICommand(WorkspaceMixin, NeedsSelectionUICommandMixin, UICommand):
    def __init__(self, parent, *, device, **kwargs):
        self._device = device
        super().__init__(parent, text=device.name, tip=_('Send selected configuration(s) to device {name}').format(name=device.name), **kwargs)

    def should_be_enabled(self):
        enabled = len(self.selection()) == 1 if isinstance(self._device, NetworkDevice) else len(self.selection()) >= 1
        return super().should_be_enabled() and enabled

    def do(self):
        if isinstance(self._device, NetworkDevice):
            configuration, = self.selection()
            uploader = ConfigurationNetworkUploader(self.mainWindow(), device=self._device, configuration=configuration, workspace=self.workspace(), mainWindow=self.mainWindow())
        else:
            uploader = ConfigurationHIDUploader(self.mainWindow(), device=self._device, configurations=self.selection(), workspace=self.workspace())
        uploader.exec_()
