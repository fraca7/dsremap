#!/usr/bin/env python3

from PyQt5 import QtWidgets
from pyqtcmd import NeedsSelectionUICommandMixin

from dsrlib.domain import commands, WorkspaceMixin, Configuration
from dsrlib.ui.mixins import MainWindowMixin

from .base import UICommand


class AddConfigurationUICommand(WorkspaceMixin, UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, icon='add', tip=_('Create a new configuration'), **kwargs)

    def do(self):
        cmd = commands.AddConfigurationCommand(workspace=self.workspace(), configuration=Configuration())
        self.history().run(cmd)


class AddConfigurationButton(MainWindowMixin, WorkspaceMixin, QtWidgets.QPushButton):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.setFlat(True)
        self.setFixedWidth(32)
        AddConfigurationUICommand(self, workspace=self.workspace(), mainWindow=self.mainWindow()).set_button(self)


class DeleteConfigurationsUICommand(NeedsSelectionUICommandMixin, WorkspaceMixin, UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, icon='del', tip=_('Delete the selected configuration'), **kwargs)

    def do(self):
        cmd = commands.DeleteConfigurationsCommand(workspace=self.workspace(), configurations=self.container().selection())
        self.history().run(cmd)


class DeleteConfigurationsButton(MainWindowMixin, WorkspaceMixin, QtWidgets.QPushButton):
    def __init__(self, *args, container, **kwargs):
        super().__init__(*args, **kwargs)
        self.setFlat(True)
        self.setFixedWidth(32)
        DeleteConfigurationsUICommand(self, workspace=self.workspace(), container=container, mainWindow=self.mainWindow()).set_button(self)
