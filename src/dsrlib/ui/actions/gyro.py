#!/usr/bin/env python3

from PyQt5 import QtCore, QtWidgets

from dsrlib.ui.mixins import MainWindowMixin
from dsrlib.domain.buttons import Buttons
from dsrlib.domain import commands

from .base import ActionWidgetMixin


class ButtonAction(QtWidgets.QAction):
    def __init__(self, parent, button):
        super().__init__(Buttons.label(button), parent)
        self._button = button

    def button(self):
        return self._button


class ButtonsButton(QtWidgets.QPushButton):
    buttonChanged = QtCore.pyqtSignal(object, bool)

    def __init__(self, *args, action, **kwargs):
        super().__init__(*args, **kwargs)
        self._action = action

        menu = QtWidgets.QMenu(self)
        for button in Buttons:
            self._addAction(menu, button)
        self.setMenu(menu)
        self._check()
        self.menu().triggered.connect(self._changed)

    def reload(self):
        for action in self.menu().actions():
            action.setChecked(action.button() in self._action.buttons())
        self._check()

    def _check(self):
        self.setText('+'.join([action.text() for action in self.menu().actions() if action.isChecked()]))

    def _changed(self, action):
        self.buttonChanged.emit(action.button(), action.isChecked())

    def _addAction(self, menu, button):
        action = ButtonAction(self, button)
        action.setCheckable(True)
        action.setChecked(button in self._action.buttons())
        menu.addAction(action)


class GyroActionWidget(MainWindowMixin, ActionWidgetMixin, QtWidgets.QWidget):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._buttons = ButtonsButton(self, action=self.action())
        self.doLayout(self.action().labelFormat(), buttons=self._buttons)
        self._buttons.buttonChanged.connect(self._toggleButton)

    def reload(self):
        self._buttons.reload()

    def _toggleButton(self, button, enabled):
        buttons = self.action().buttons()
        if enabled and button not in buttons:
            buttons.append(button)
        elif button in buttons and not enabled:
            buttons.remove(button)

        if buttons != self.action().buttons():
            cmd = commands.SetGyroButtonsCommand(action=self.action(), buttons=buttons)
            self.history().run(cmd)
