#!/usr/bin/env python3

from PyQt5 import QtWidgets

from dsrlib.settings import Settings

from .pages.arduino import ArduinoWelcomePage, ArduinoAvrdudePage, ArduinoResetPage, ArduinoFindSerialPage
from .base import Wizard


class HexUploader(Wizard):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.setOption(self.NoCancelButton)

        btn = QtWidgets.QPushButton(_('Skip'), self)
        self.setButton(self.CustomButton1, btn)
        self.setOption(self.HaveCustomButton1)
        btn.clicked.connect(self.reject)

    def setupPages(self):
        raise NotImplementedError # Make pylint happy

    def onSuccess(self):
        Settings().setFirmwareUploaded()


class HexUploaderWizard(HexUploader):
    def setupPages(self):
        if Settings().avrdude() is None:
            self.addPage(ArduinoAvrdudePage(self, mainWindow=self.mainWindow()))
        self.addPage(ArduinoResetPage(self, mainWindow=self.mainWindow()))
        self.addPage(ArduinoFindSerialPage(self, mainWindow=self.mainWindow()))


class FirstLaunchWizard(HexUploaderWizard):
    def setupPages(self):
        self.addPage(ArduinoWelcomePage(self, mainWindow=self.mainWindow()))
        super().setupPages()
