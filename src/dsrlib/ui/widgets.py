#!/usr/bin/env python3

from PyQt5 import QtCore, QtWidgets

from dsrlib.domain.buttons import Buttons


class ButtonAction(QtWidgets.QAction):
    def __init__(self, parent, button):
        super().__init__(Buttons.label(button), parent)
        self._button = button

    def button(self):
        return self._button


class ButtonListWidget(QtWidgets.QPushButton):
    buttonChanged = QtCore.pyqtSignal(object, bool)

    def __init__(self, parent):
        super().__init__(parent)

        menu = QtWidgets.QMenu(self)
        for button in Buttons:
            self._addAction(menu, button)
        self.setMenu(menu)
        self.reload()
        self.menu().triggered.connect(self._changed)

    def reload(self):
        for action in self.menu().actions():
            action.setChecked(self.isChecked(action.button()))
        self._check()

    def _check(self):
        self.setText('+'.join([button.name for button in Buttons if self.isChecked(button)]))

    def _changed(self, action):
        self.buttonChanged.emit(action.button(), action.isChecked())
        self.reload()

    def _addAction(self, menu, button):
        action = ButtonAction(self, button)
        action.setCheckable(True)
        action.setChecked(self.isChecked(button))
        menu.addAction(action)

    def isChecked(self, button):
        raise NotImplementedError
