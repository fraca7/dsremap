#!/usr/bin/env python3

from PyQt5 import QtWidgets

from dsrlib.ui.utils import LayoutBuilder

from .pageids import PageId
from .base import Page


class DeviceChoicePage(Page):
    ID = PageId.DeviceType

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._nextId = -1

        bld = LayoutBuilder(self)
        with bld.vbox() as layout:
            layout.addStretch(1)
            self._addButton(layout, _('Arduino Leonardo'), PageId.ArduinoAvrdude, True)
            self._addButton(layout, _('Raspberry Pi Zero W'), PageId.PiZeroFind)
            self._addButton(layout, _('Do that later'), DeviceNotConfiguredPage.ID)
            layout.addStretch(1)

    def _addButton(self, layout, label, pageId, checked=False):
        btn = QtWidgets.QRadioButton(label, self)
        btn.setChecked(checked)
        if checked:
            self._nextId = pageId

        def setNextId():
            self._nextId = pageId
        btn.clicked.connect(setNextId)
        layout.addWidget(btn)

    def initializePage(self):
        self.setTitle(_('Device type'))
        self.setSubTitle(_('Choose what kind of device you want to configure'))

    def nextId(self):
        return DeviceNotConfiguredPage.ID


class DeviceNotConfiguredPage(Page):
    ID = PageId.DeviceNotConfigured

    def initializePage(self):
        self.setTitle(_('Skipped'))
        self.setSubTitle(_('Skipped initial device configuration. You can configure your devices later from the Devices menu.'))
        self.wizard().onSuccess()

    def nextId(self):
        return -1
