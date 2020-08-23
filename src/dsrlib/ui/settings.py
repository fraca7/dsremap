#!/usr/bin/env python3

import platform
import contextlib

from PyQt5 import QtGui, QtWidgets

from dsrlib.settings import Settings
from dsrlib.ui.mixins import MainWindowMixin

from .utils import LayoutBuilder, getOpenFilename


class FilenameSettingWidget(QtWidgets.QWidget):
    def __init__(self, parent, *, name, placeholder='', default=''):
        super().__init__(parent)
        self._name = name
        self._default = default

        self._filename = QtWidgets.QLineEdit(self)
        self._filename.setPlaceholderText(placeholder)
        btn = QtWidgets.QPushButton(_('Browse'), self)

        bld = LayoutBuilder(self)
        with bld.hbox() as hbox:
            hbox.addWidget(self._filename, stretch=1)
            hbox.addWidget(btn)

        btn.clicked.connect(self._browse)

    def name(self):
        return self._name

    def load(self, settings):
        self._filename.setText(settings.value(self._name, self._default))

    def save(self, settings):
        settings.setValue(self._name, self._filename.text())

    def _browse(self):
        filename = getOpenFilename(self, 'Settings', 'exe' if platform.system() == 'Windows' else '')
        if filename is not None:
            self._filename.setText(filename)


class BooleanSettingWidget(QtWidgets.QCheckBox):
    def __init__(self, text, parent, *, name, default):
        super().__init__(text, parent)
        self._name = name
        self._default = default

    def name(self):
        return self._name

    def load(self, settings):
        self.setChecked(settings.booleanValue(self._name, self._default))

    def save(self, settings):
        settings.setBooleanValue(self._name, self.isChecked())



class SettingsPage(QtWidgets.QWidget):
    def __init__(self, parent):
        super().__init__(parent)
        self._group = []
        self._items = []

        self.setLayout(QtWidgets.QFormLayout())
        self.layout().setFieldGrowthPolicy(self.layout().ExpandingFieldsGrow)

    @contextlib.contextmanager
    def begin(self, *args):
        group, self._group = self._group, self._group + list(args)
        try:
            yield self
        finally:
            self._group = group

    def addSetting(self, widget, label=None):
        self._items.append((tuple(self._group), widget))
        if label:
            self.layout().addRow(label, widget)
        else:
            self.layout().addRow(widget)

    def addFilenameSetting(self, name, default, label, **kwargs):
        widget = FilenameSettingWidget(self, name=name, default=default, **kwargs)
        self.addSetting(widget, label)

    def addBooleanSetting(self, name, default, label):
        widget = BooleanSettingWidget(label, self, name=name, default=default)
        self.addSetting(widget)

    def load(self, settings):
        for group, widget in self._items:
            with settings.grouped(*group):
                widget.load(settings)

    def save(self, settings):
        for group, widget in self._items:
            with settings.grouped(*group):
                widget.save(settings)

    def icon(self):
        raise NotImplementedError

    def text(self):
        raise NotImplementedError


class MiscSettingsPage(SettingsPage):
    def __init__(self, parent):
        super().__init__(parent)

        with self.begin('Misc'):
            self.addFilenameSetting('avrdude', '', _('Path to avrdude'), placeholder=_('Autodetect'))
            self.addBooleanSetting('devmode', False, _('Developper mode'))

    def icon(self):
        return QtGui.QIcon(':icons/config.svg')

    def text(self):
        return _('Miscellaneous')


class SettingsDialog(MainWindowMixin, QtWidgets.QDialog):
    def __init__(self, parent, **kwargs):
        super().__init__(parent, **kwargs)

        self._list = QtWidgets.QListWidget(self)
        self._list.setViewMode(self._list.IconMode)
        self._list.setFlow(self._list.TopToBottom)
        self._content = QtWidgets.QStackedWidget(self)

        btnOK = QtWidgets.QPushButton(_('Done'), self)
        btnCancel = QtWidgets.QPushButton(_('Cancel'), self)

        bld = LayoutBuilder(self)
        with bld.vbox():
            with bld.hbox() as hbox:
                hbox.addWidget(self._list)
                hbox.addWidget(self._content, stretch=1)
            with bld.hbox() as buttons:
                buttons.addStretch(1)
                buttons.addWidget(btnCancel)
                buttons.addWidget(btnOK)

        settings = Settings()
        with settings.grouped('UIState'):
            if settings.contains('PreferencesGeometry'):
                self.restoreGeometry(settings.value('PreferencesGeometry'))
            else:
                self.resize(800, 600)

        btnOK.clicked.connect(self.accept)
        btnCancel.clicked.connect(self.reject)

        self.addPage(MiscSettingsPage(self))

        self._list.setFixedWidth(self._list.sizeHintForColumn(0) + 5)
        self._list.itemSelectionChanged.connect(self._switchPage)
        self._list.item(0).setSelected(True)

        with settings.grouped('Settings'):
            for index in range(self._content.count()):
                self._content.widget(index).load(settings)

    def addPage(self, page):
        item = QtWidgets.QListWidgetItem(self._list)
        item.setIcon(page.icon())
        item.setText(page.text())
        self._content.addWidget(page)

    def accept(self):
        settings = Settings()
        with settings.grouped('UIState'):
            settings.setValue('PreferencesGeometry', self.saveGeometry())
        with settings.grouped('Settings'):
            for index in range(self._content.count()):
                self._content.widget(index).save(settings)
        self.mainWindow().settingsChanged.emit()
        return super().accept()

    def reject(self):
        settings = Settings()
        with settings.grouped('UIState'):
            settings.setValue('PreferencesGeometry', self.saveGeometry())
        return super().reject()

    def _switchPage(self):
        self._content.setCurrentIndex(self._list.currentRow())
