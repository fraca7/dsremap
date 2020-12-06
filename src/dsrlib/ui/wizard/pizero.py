#!/usr/bin/env python3

from .pages.pizero import PiZeroPlugPage, PiZeroPairHostPage, PiZeroWaitDSPage, PiZeroPairDSPage, PiZeroFinalPage
from .base import Wizard


class PairingWizard(Wizard):
    def __init__(self, *args, device, enumerator, **kwargs):
        super().__init__(*args, **kwargs)
        self._enumerator = enumerator
        self._device = device
        self._dualshock = None
        self._linkkey = None
        self._dongle = None

    def setupPages(self):
        self.addPage(PiZeroPlugPage(self, mainWindow=self.mainWindow()))
        self.addPage(PiZeroPairHostPage(self, mainWindow=self.mainWindow()))
        self.addPage(PiZeroWaitDSPage(self, enumerator=self._enumerator, mainWindow=self.mainWindow()))
        self.addPage(PiZeroPairDSPage(self, mainWindow=self.mainWindow()))
        self.addPage(PiZeroFinalPage(self, mainWindow=self.mainWindow()))

    def device(self):
        return self._device

    def setDualshock(self, dev):
        self._dualshock = dev

    def dualshock(self):
        return self._dualshock

    def setLinkKey(self, key):
        self._linkkey = key

    def linkKey(self):
        return self._linkkey

    def setDongle(self, addr):
        self._dongle = addr

    def dongle(self):
        return self._dongle
