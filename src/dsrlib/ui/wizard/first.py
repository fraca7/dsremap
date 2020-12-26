#!/usr/bin/env python3

from .pages.common import DeviceChoicePage, DeviceNotConfiguredPage
from .base import Wizard, ManifestWizardMixin
from .arduino import HexUploaderWizardMixin
from .pizero import PairingWizardMixin


class FirstLaunchWizard(HexUploaderWizardMixin, ManifestWizardMixin, PairingWizardMixin, Wizard):
    def setupPages(self):
        self.addPage(DeviceChoicePage(mainWindow=self.mainWindow()))
        self.addPage(DeviceNotConfiguredPage(mainWindow=self.mainWindow()))
        super().setupPages()
