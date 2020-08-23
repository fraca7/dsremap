#!/usr/bin/env python3

from pyqtcmd import Command


class SetActionAttributesCommand(Command):
    def __init__(self, *, action, **kwargs):
        super().__init__()
        self._action = action
        self._old = {name: getattr(action, name)() for name in kwargs}
        self._new = kwargs

    def _apply(self, attr):
        for name, value in attr.items():
            getattr(self._action, 'set%s' % name.capitalize())(value)

    def do(self):
        self._apply(self._new)

    def undo(self):
        self._apply(self._old)


class SetGyroButtonsCommand(Command):
    def __init__(self, *, action, buttons):
        super().__init__()
        self._action = action
        self._old = action.buttons()
        self._new = buttons

    def do(self):
        self._action.setButtons(self._new)

    def undo(self):
        self._action.setButtons(self._old)
