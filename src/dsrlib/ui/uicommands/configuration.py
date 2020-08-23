#!/usr/bin/env python3

from PyQt5 import QtGui, QtWidgets
from pyqtcmd import NeedsSelectionUICommandMixin

from dsrlib.domain import commands, ConfigurationMixin
from dsrlib.domain.actions import InvertPadAxisAction, SwapAxisAction, GyroAction, CustomAction
from dsrlib.ui.mixins import MainWindowMixin, DeveloperModeItemMixin
from dsrlib.ui.ide import Editor

from .base import UICommand


class AddInvertPadAxisActionUICommand(ConfigurationMixin, UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, text=_('Invert pad axis'), tip=_('Add an action to invert an axis'), **kwargs)

    def do(self):
        cmd = commands.AddActionCommand(configuration=self.configuration(), action=InvertPadAxisAction())
        self.history().run(cmd)


class AddSwapAxisActionUICommand(ConfigurationMixin, UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, text=_('Swap axis'), tip=_('Add an action to swap two axis'), **kwargs)

    def do(self):
        cmd = commands.AddActionCommand(configuration=self.configuration(), action=SwapAxisAction())
        self.history().run(cmd)


class AddGyroActionUICommand(ConfigurationMixin, UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, text=_('Gyro aiming'), tip=_('Add an action to implement gyro aiming'), **kwargs)

    def do(self):
        cmd = commands.AddActionCommand(configuration=self.configuration(), action=GyroAction())
        self.history().run(cmd)


class AddCustomActionUICommand(DeveloperModeItemMixin, ConfigurationMixin, UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, text=_('Custom action'), tip=_('Add a custom action and open the code editor'), **kwargs)

    def do(self):
        action = CustomAction()

        editor = Editor(self.mainWindow(), action=action)
        if editor.exec_() == editor.Accepted:
            action.setSource(editor.source())
            cmd = commands.AddActionCommand(configuration=self.configuration(), action=action)
            self.history().run(cmd)


class AddActionButton(MainWindowMixin, ConfigurationMixin, QtWidgets.QPushButton):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.clicked.connect(self.showMenu)

        self.setIcon(QtGui.QIcon(':icons/add.svg'))
        tip = _('Add a remapping action')
        self.setToolTip(tip)
        self.setStatusTip(tip)
        self.setFlat(True)
        self.setFixedWidth(32)

        menu = QtWidgets.QMenu(self)
        menu.addAction(AddInvertPadAxisActionUICommand(self, configuration=self.configuration(), mainWindow=self.mainWindow()))
        menu.addAction(AddSwapAxisActionUICommand(self, configuration=self.configuration(), mainWindow=self.mainWindow()))
        menu.addAction(AddGyroActionUICommand(self, configuration=self.configuration(), mainWindow=self.mainWindow()))
        menu.addAction(AddCustomActionUICommand(self, configuration=self.configuration(), mainWindow=self.mainWindow()))
        self.setMenu(menu)


class DeleteActionsUICommand(NeedsSelectionUICommandMixin, ConfigurationMixin, UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, icon='del', tip=_('Delete the selected action'), **kwargs)

    def do(self):
        cmd = commands.DeleteActionsCommand(configuration=self.configuration(), actions=self.container().selection())
        self.history().run(cmd)


class DeleteActionsButton(MainWindowMixin, ConfigurationMixin, QtWidgets.QPushButton):
    def __init__(self, parent, *, container, **kwargs):
        super().__init__(parent, **kwargs)
        DeleteActionsUICommand(self, configuration=self.configuration(), container=container, mainWindow=self.mainWindow()).set_button(self)
        self.setFlat(True)
        self.setFixedWidth(32)


class ConvertToCustomActionUICommand(DeveloperModeItemMixin, NeedsSelectionUICommandMixin, ConfigurationMixin, UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, icon='convert', tip=_('Convert the selected action to a custom one and open the code editor'), **kwargs)

    def should_be_enabled(self):
        selection = self.selection()
        if len(selection) != 1:
            return False
        item, = selection
        if isinstance(item, CustomAction):
            return False
        return super().should_be_enabled()

    def do(self):
        proto, = self.selection()
        action = CustomAction()
        action.setSource(proto.source())

        editor = Editor(self.mainWindow(), action=action)
        if editor.exec_() == editor.Accepted:
            action.setSource(editor.source())
            cmd = commands.AddActionCommand(configuration=self.configuration(), action=action)
            self.history().run(cmd)


class ConvertToCustomActionButton(MainWindowMixin, ConfigurationMixin, QtWidgets.QPushButton):
    def __init__(self, parent, *, container, **kwargs):
        super().__init__(parent, **kwargs)
        ConvertToCustomActionUICommand(self, configuration=self.configuration(), container=container, mainWindow=self.mainWindow()).set_button(self)
        self.setFlat(True)
        self.setFixedWidth(32)
