#!/usr/bin/env python3

from PyQt5 import QtCore, QtGui

from dsrlib.ui.wizard import HexUploaderWizard, SetupSDWizard
from dsrlib.ui.about import AboutDialog
from dsrlib.ui.settings import SettingsDialog
from dsrlib.ui.resdl import ResourceDownloader
from dsrlib.meta import Meta

from .base import UICommand


class UpdateHexUICommand(UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, text=_('Update Arduino firmware'), tip=_('Program the microcontroller with DSRemap firmware (if you use an Arduino)'), **kwargs)

    def do(self):
        dlg = ResourceDownloader(self.mainWindow(), mainWindow=self.mainWindow(), path=('leonardo', 'firmware'))
        if dlg.exec_() == dlg.Accepted:
            wizard = HexUploaderWizard(self.mainWindow(), mainWindow=self.mainWindow())
            wizard.exec_()


class SetupSDCardUICommand(UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, text=_('Setup a new SD card for the RPi0 W'), tip=_('Create an image file for the Raspberry Pi Zero W'), **kwargs)

    def do(self):
        dlg = ResourceDownloader(self.mainWindow(), mainWindow=self.mainWindow(), path=('rpi0w', 'image'))
        if dlg.exec_() == dlg.Accepted:
            wizard = SetupSDWizard(self.mainWindow(), mainWindow=self.mainWindow())
            wizard.exec_()


class ShowAboutDialogUICommand(UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, text=_('About {appname}...').format(appname=Meta.appName()), **kwargs)
        self.setMenuRole(self.AboutRole)

    def do(self):
        dlg = AboutDialog(self.mainWindow())
        dlg.exec_()


class ShowSettingsDialogUICommand(UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, text=_('Settings...'), **kwargs)
        self.setMenuRole(self.PreferencesRole)

    def do(self):
        dlg = SettingsDialog(self.mainWindow(), mainWindow=self.mainWindow())
        if dlg.exec_() == dlg.Accepted:
            self.mainWindow().reloadSettings()


class OpenDocsUICommand(UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, text=_('Open documentation'), icon='help', **kwargs)

    def do(self):
        QtGui.QDesktopServices.openUrl(QtCore.QUrl(Meta.documentationUrl()))
