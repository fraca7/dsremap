#!/usr/bin/env python3

from PyQt5 import QtCore, QtGui, QtWidgets

from dsrlib.domain import commands
from dsrlib.ui.mixins import MainWindowMixin
from dsrlib.ui.utils import LayoutBuilder
from dsrlib.ui.ide import Editor

from .base import ActionWidgetMixin


class ErrorMessage(QtWidgets.QWidget):
    def __init__(self, parent):
        super().__init__(parent)
        self._text = ''

    def setText(self, text):
        self._text = text
        if text:
            self.show()
        else:
            self.hide()
        self.update()

    def sizeHint(self):
        mt = QtGui.QFontMetrics(self.font())
        return QtCore.QSize(0, mt.height())

    def paintEvent(self, event): # pylint: disable=W0613
        painter = QtGui.QPainter(self)
        painter.setPen(QtCore.Qt.red)
        mt = painter.fontMetrics()
        text = mt.elidedText(self._text or '', QtCore.Qt.ElideRight, self.width())
        painter.drawText(self.rect(), QtCore.Qt.AlignLeft, text)


class CustomActionWidget(MainWindowMixin, ActionWidgetMixin, QtWidgets.QWidget):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self._name = QtWidgets.QLabel(self)
        self._error = ErrorMessage(self)
        self._btn = QtWidgets.QPushButton(_('Edit'), self)

        bld = LayoutBuilder(self)
        with bld.vbox() as vbox:
            with bld.hbox() as hbox:
                hbox.addWidget(self._name, stretch=1)
                hbox.addWidget(self._btn)
            vbox.addWidget(self._error)

        self._btn.clicked.connect(self._editCode)
        self.reload()

    def reload(self):
        self._name.setText(self.action().label())
        self._error.setText(self.action().error())

        # The tree widget updates the cell size but not the widget...
        self.resize(self.width(), self.sizeHint().height())
        self.geometryChanged.emit()

    def _editCode(self):
        editor = Editor(self.mainWindow(), action=self.action())
        if editor.exec_() == editor.Accepted:
            cmd = commands.SetActionAttributesCommand(action=self.action(), source=editor.source())
            self.history().run(cmd)
