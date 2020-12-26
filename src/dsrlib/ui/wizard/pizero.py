#!/usr/bin/env python3

from .pages.pizero import PiZeroManifestDownloadPage, PiZeroImageDownloadPage, PiZeroWifiPage, PiZeroCopyPage, PiZeroBurnPage, PiZeroFindPage, PiZeroPlugPage, \
     PiZeroPairHostPage, PiZeroWaitDSPage, PiZeroPairDSPage, PiZeroFinalPage
from .base import Wizard


class PairingWizardMixin:
    def __init__(self, *args, enumerator, device=None, **kwargs):
        self._enumerator = enumerator
        self._device = device
        self._dualshock = None
        self._linkkey = None
        self._dongle = None
        super().__init__(*args, **kwargs)

    def setupPages(self):
        self.addPage(PiZeroFindPage(self, enumerator=self._enumerator, mainWindow=self.mainWindow()))
        self.addPage(PiZeroPlugPage(self, mainWindow=self.mainWindow()))
        self.addPage(PiZeroPairHostPage(self, mainWindow=self.mainWindow()))
        self.addPage(PiZeroWaitDSPage(self, enumerator=self._enumerator, mainWindow=self.mainWindow()))
        self.addPage(PiZeroPairDSPage(self, mainWindow=self.mainWindow()))
        self.addPage(PiZeroFinalPage(self, mainWindow=self.mainWindow()))
        super().setupPages()

    def setDevice(self, device):
        self._device = device

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


class SetupSDWizardMixin:
    def __init__(self, *args, **kwargs):
        self._wifi = (None, None)
        super().__init__(*args, **kwargs)

    def setupPages(self):
        self.addPage(PiZeroManifestDownloadPage(self, mainWindow=self.mainWindow()))
        self.addPage(PiZeroImageDownloadPage(self, mainWindow=self.mainWindow()))
        self.addPage(PiZeroWifiPage(self, mainWindow=self.mainWindow()))
        self.addPage(PiZeroCopyPage(self, mainWindow=self.mainWindow()))
        self.addPage(PiZeroBurnPage(self, mainWindow=self.mainWindow()))
        super().setupPages()

    def wifi(self):
        return self._wifi

    def setWifi(self, ssid, password):
        self._wifi = (ssid, password)


class PairingWizard(PairingWizardMixin, Wizard):
    pass


class SetupSDWizard(SetupSDWizardMixin, Wizard):
    pass
