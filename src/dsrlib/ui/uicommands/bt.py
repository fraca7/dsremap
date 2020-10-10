#!/usr/bin/env python3

from dsrlib.ui.pairer import PairingWizard

from .base import UICommand


class PairUICommand(UICommand):
    def __init__(self, parent, *, device, enumerator, **kwargs):
        super().__init__(parent, text=device.name, tip=_('Pair a DualShock 4 and a PS4 to this DSRemap-enabled device'), **kwargs)
        self._device = device
        self._enumerator = enumerator

    def do(self):
        wizard = PairingWizard(self.mainWindow(), self._device, self._enumerator)
        wizard.exec_()
