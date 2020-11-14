#!/usr/bin/env python3

from PyQt5 import QtWidgets

from dsrlib.ui.mixins import MainWindowMixin
from dsrlib.ui.widgets import ButtonListWidget
from dsrlib.domain import commands

from .base import ActionWidgetMixin


class GyroActionButtonListWidget(ButtonListWidget):
    def __init__(self, *args, action, **kwargs):
        self._action = action
        super().__init__(*args, **kwargs)

    def isChecked(self, button):
        return button in self._action.buttons()


class GyroActionWidget(MainWindowMixin, ActionWidgetMixin, QtWidgets.QWidget):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._buttons = GyroActionButtonListWidget(self, action=self.action())
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
