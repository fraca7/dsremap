#!/usr/bin/env python3

from dsrlib.ui.hexuploader import HexUploaderWizard
from dsrlib.ui.about import AboutDialog
from dsrlib.ui.settings import SettingsDialog
from dsrlib.meta import Meta

from .base import UICommand


class UpdateHexUICommand(UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, text=_('Update firmware'), tip=_('Program the microcontroller with DSRemap firmware'), **kwargs)

    def do(self):
        wizard = HexUploaderWizard(self.mainWindow())
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
