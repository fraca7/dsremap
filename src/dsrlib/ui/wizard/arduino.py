#!/usr/bin/env python3

from .pages.arduino import ArduinoAvrdudePage, ArduinoResetPage, ArduinoFindSerialPage, ArduinoManifestDownloadPage, ArduinoDownloadPage
from .base import Wizard


class HexUploaderWizardMixin:
    def setupPages(self):
        self.addPage(ArduinoAvrdudePage(self, mainWindow=self.mainWindow()))
        self.addPage(ArduinoManifestDownloadPage(self, mainWindow=self.mainWindow()))
        self.addPage(ArduinoDownloadPage(self, mainWindow=self.mainWindow()))
        self.addPage(ArduinoResetPage(self, mainWindow=self.mainWindow()))
        self.addPage(ArduinoFindSerialPage(self, mainWindow=self.mainWindow()))
        super().setupPages()


class HexUploaderWizard(HexUploaderWizardMixin, Wizard):
    pass
