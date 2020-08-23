#!/usr/bin/env python3

import io

from PyQt5 import QtCore, QtWidgets

from dsrlib.ui.utils import LayoutBuilder


class ActionWidgetMixin:
    geometryChanged = QtCore.pyqtSignal()

    def __init__(self, *args, action, **kwargs):
        super().__init__(*args, **kwargs)
        self._action = action
        self._action.changed.connect(self.reload)

    def action(self):
        return self._action

    def reload(self):
        raise NotImplementedError

    def doLayout(self, fmt, **kwargs):
        bld = LayoutBuilder(self)
        with bld.hbox() as layout:
            text = io.StringIO()
            state = 0
            for char in fmt:
                if state == 0:
                    if char == '{':
                        if text.tell():
                            label = QtWidgets.QLabel(text.getvalue())
                            layout.addWidget(label)
                        name = io.StringIO()
                        state = 1
                    else:
                        text.write(char)
                elif state == 1:
                    if char == '}':
                        name = name.getvalue()
                        layout.addWidget(kwargs.pop(name))
                        text = io.StringIO()
                        state = 0
                    else:
                        name.write(char)
            layout.addStretch(1)
