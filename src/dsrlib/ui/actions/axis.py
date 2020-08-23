#!/usr/bin/env python3

from PyQt5 import QtWidgets

from dsrlib.domain import commands
from dsrlib.domain.actions import Axis
from dsrlib.ui.mixins import MainWindowMixin
from dsrlib.ui.utils import EnumComboBox

from .base import ActionWidgetMixin


class InvertPadAxisActionWidget(MainWindowMixin, ActionWidgetMixin, QtWidgets.QWidget):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self._pad = QtWidgets.QComboBox(self)
        self._pad.addItem(self.action().padLabel(self.action().LPAD), self.action().LPAD)
        self._pad.addItem(self.action().padLabel(self.action().RPAD), self.action().RPAD)
        self._axis = QtWidgets.QComboBox(self)
        self._axis.addItem(self.action().axisLabel(self.action().XAXIS), self.action().XAXIS)
        self._axis.addItem(self.action().axisLabel(self.action().YAXIS), self.action().YAXIS)
        self.reload()

        self._pad.currentIndexChanged.connect(self._changePad)
        self._axis.currentIndexChanged.connect(self._changeDir)

        self.doLayout(self.action().labelFormat(), pad=self._pad, axis=self._axis)

    def _changePad(self, _):
        pad = self._pad.currentData()
        if pad != self.action().pad():
            cmd = commands.SetActionAttributesCommand(action=self.action(), pad=pad)
            self.history().run(cmd)

    def _changeDir(self, _):
        axis = self._axis.currentData()
        if axis != self.action().axis():
            cmd = commands.SetActionAttributesCommand(action=self.action(), axis=axis)
            self.history().run(cmd)

    def reload(self):
        self._pad.setCurrentIndex(self.action().pad())
        self._axis.setCurrentIndex(self.action().axis())


class SwapAxisActionWidget(MainWindowMixin, ActionWidgetMixin, QtWidgets.QWidget):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self._axis1 = EnumComboBox(self, enum=Axis, value=self.action().axis1())
        self._axis2 = EnumComboBox(self, enum=Axis, value=self.action().axis2())

        self._axis1.valueChanged.connect(self._axis1Changed)
        self._axis2.valueChanged.connect(self._axis2Changed)

        self.doLayout(self.action().labelFormat(), axis1=self._axis1, axis2=self._axis2)

    def _axis1Changed(self, value):
        if value != self.action().axis1():
            cmd = commands.SetActionAttributesCommand(action=self.action(), axis1=value)
            self.history().run(cmd)

    def _axis2Changed(self, value):
        if value != self.action().axis2():
            cmd = commands.SetActionAttributesCommand(action=self.action(), axis2=value)
            self.history().run(cmd)

    def reload(self):
        self._axis1.setValue(self.action().axis1())
        self._axis2.setValue(self.action().axis2())
