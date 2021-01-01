#!/usr/bin/env python3

from PyQt5 import QtWidgets

from dsrlib.domain import commands
from dsrlib.ui.mixins import MainWindowMixin
from dsrlib.ui.widgets import ButtonListWidget

from .base import ActionWidgetMixin


class DisableButtonActionButtonListWidget(ButtonListWidget):
    def __init__(self, *args, action, **kwargs):
        self._action = action
        super().__init__(*args, **kwargs)

    def isChecked(self, button):
        return button == self._action.button()


class DisableButtonActionWidget(MainWindowMixin, ActionWidgetMixin, QtWidgets.QWidget):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._button = DisableButtonActionButtonListWidget(self, action=self.action())
        self.doLayout(self.action().labelFormat(), button=self._button)
        self._button.buttonChanged.connect(self._toggleButton)

    def reload(self):
        self._button.reload()

    def _toggleButton(self, button, enabled):
        if enabled and button != self.action().button():
            cmd = commands.SetActionAttributesCommand(action=self.action(), button=button)
            self.history().run(cmd)
