#!/usr/bin/env python3

import os
import contextlib

from PyQt5 import QtCore, QtWidgets

from dsrlib.settings import Settings


class LayoutBuilder:
    def __init__(self, target):
        self.target = target
        self._stack = []

    @contextlib.contextmanager
    def _layout(self, cls, *args, **kwargs):
        layout = cls()
        self._stack.append(layout)
        try:
            yield layout
        finally:
            self._pop(*args, **kwargs)

    def _pop(self, *args, **kwargs):
        layout = self._stack.pop()
        if self._stack:
            parent = self._stack[-1]
            if isinstance(layout, QtWidgets.QSplitter):
                parent.addWidget(layout)
            else:
                if isinstance(parent, QtWidgets.QSplitter):
                    container = QtWidgets.QWidget(parent)
                    container.setLayout(layout)
                    parent.addWidget(container)
                else:
                    parent.addLayout(layout, *args, **kwargs)
        elif isinstance(self.target, QtWidgets.QMainWindow):
            if isinstance(layout, QtWidgets.QSplitter):
                self.target.setCentralWidget(layout)
            else:
                container = QtWidgets.QWidget(self.target)
                container.setLayout(layout)
                self.target.setCentralWidget(container)
        else:
            if isinstance(layout, QtWidgets.QSplitter):
                layout2 = QtWidgets.QHBoxLayout()
                layout2.setContentsMargins(0, 0, 0, 0)
                layout2.addWidget(layout)
                self.target.setLayout(layout2)
            else:
                self.target.setLayout(layout)

    @contextlib.contextmanager
    def hbox(self, *args, **kwargs): # pragma: no cover
        with self._layout(QtWidgets.QHBoxLayout, *args, **kwargs) as layout:
            layout.setContentsMargins(1, 1, 1, 1)
            layout.setSpacing(1)
            yield layout

    @contextlib.contextmanager
    def vbox(self, *args, **kwargs): # pragma: no cover
        with self._layout(QtWidgets.QVBoxLayout, *args, **kwargs) as layout:
            layout.setContentsMargins(1, 1, 1, 1)
            layout.setSpacing(1)
            yield layout

    def stack(self, *args, **kwargs): # pragma: no cover
        return self._layout(QtWidgets.QStackedLayout, *args, **kwargs)

    def form(self, *args, **kwargs):
        class _FormLayout(QtWidgets.QFormLayout):
            def addLayout(self, layout):
                self.addRow(layout)

            def addRow(self, label, widget=None): # pylint: disable=C0111
                if isinstance(label, str):
                    label = QtWidgets.QLabel(label)
                    label.setSizePolicy(QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Expanding)
                    label.setAlignment(QtCore.Qt.AlignVCenter)
                if widget is None:
                    super().addRow(label)
                else:
                    super().addRow(label, widget)

        return self._layout(_FormLayout, *args, **kwargs)

    def split(self, *args, **kwargs): # pragma: no cover
        return self._layout(QtWidgets.QSplitter, *args, **kwargs)


def getSaveFilename(parent, domain, extension):
    with Settings().grouped('Paths') as settings:
        path = QtCore.QStandardPaths.writableLocation(QtCore.QStandardPaths.DocumentsLocation)
        sname = 'save_%s' % domain
        if settings.contains(sname):
            path = settings.value(sname)

        while True:
            name, dummy = QtWidgets.QFileDialog.getSaveFileName(parent, _('Save'), path, '*.%s' % extension, options=QtWidgets.QFileDialog.DontConfirmOverwrite)
            if not name:
                return None
            if not name.endswith('.%s' % extension):
                name = '%s.%s' % (name, extension)
            if os.path.exists(name):
                resp = QtWidgets.QMessageBox.question(parent,
                                                      _('Overwrite file?'),
                                                      _('This file already exists. Overwrite?'),
                                                      QtWidgets.QMessageBox.Yes|QtWidgets.QMessageBox.No|QtWidgets.QMessageBox.Cancel)
                if resp == QtWidgets.QMessageBox.Yes:
                    settings.setValue(sname, os.path.dirname(name))
                    return name
                if resp == QtWidgets.QMessageBox.No:
                    continue
                return None
            settings.setValue(sname, os.path.dirname(name))
            return name


def getOpenFilename(parent, domain, extension):
    with Settings().grouped('Paths') as settings:
        path = QtCore.QStandardPaths.writableLocation(QtCore.QStandardPaths.DocumentsLocation)
        sname = 'open_%s' % domain
        if settings.contains(sname):
            path = settings.value(sname)

        name, dummy = QtWidgets.QFileDialog.getOpenFileName(parent, _('Open file'), path, '*.%s' % extension if extension else '')
        if name:
            settings.setValue(sname, os.path.dirname(name))
            return name
        return None


class EnumComboBox(QtWidgets.QComboBox):
    valueChanged = QtCore.pyqtSignal(object)

    def __init__(self, *args, enum, value=None, **kwargs):
        super().__init__(*args, **kwargs)
        self._enum = enum
        for item in enum:
            self.addItem(enum.label(item), item)
        if value is not None:
            self.setValue(value)
        self.currentIndexChanged.connect(self._emit)

    def setValue(self, value):
        for index, item in enumerate(self._enum):
            if value == item:
                self.setCurrentIndex(index)
                break
        else:
            raise ValueError('Value "%s" not found in enum' % str(value))

    def _emit(self, _):
        self.valueChanged.emit(self.currentData())
