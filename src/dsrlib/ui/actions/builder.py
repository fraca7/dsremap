#!/usr/bin/env python3

from dsrlib.domain.actions import ActionVisitor

from .axis import InvertPadAxisActionWidget, SwapAxisActionWidget
from .gyro import GyroActionWidget
from .buttons import DisableButtonActionWidget
from .custom import CustomActionWidget


class ActionWidgetBuilder(ActionVisitor):
    def __init__(self, parent, mainWindow):
        self._parent = parent
        self._mainWindow = mainWindow

    def _acceptDisableButtonAction(self, action):
        return DisableButtonActionWidget(self._parent, action=action, mainWindow=self._mainWindow)

    def _acceptInvertPadAxisAction(self, action):
        return InvertPadAxisActionWidget(self._parent, action=action, mainWindow=self._mainWindow)

    def _acceptSwapAxisAction(self, action):
        return SwapAxisActionWidget(self._parent, action=action, mainWindow=self._mainWindow)

    def _acceptGyroAction(self, action):
        return GyroActionWidget(self._parent, action=action, mainWindow=self._mainWindow)

    def _acceptCustomAction(self, action):
        return CustomActionWidget(self._parent, action=action, mainWindow=self._mainWindow)
