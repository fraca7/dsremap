#!/usr/bin/env python3

from PyQt5 import QtCore, QtGui, QtWidgets

from dsrlib.domain import commands, WorkspaceMixin, ConfigurationMixin

from .mixins import MainWindowMixin
from .utils import LayoutBuilder
from .uicommands import AddConfigurationButton, DeleteConfigurationsButton
from .configuration import ConfigurationView


class ConfigurationItemWidget(MainWindowMixin, ConfigurationMixin, QtWidgets.QWidget):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self._check = QtWidgets.QCheckBox(self)
        self._check.setFixedWidth(32)
        self._thumbnail = QtWidgets.QLabel(self)
        self._name = QtWidgets.QLabel(self)

        bld = LayoutBuilder(self)
        with bld.hbox() as hbox:
            hbox.addWidget(self._check)
            hbox.addWidget(self._thumbnail)
            hbox.addSpacing(10)
            hbox.addWidget(self._name)
            hbox.addStretch(1)

        self.reload()
        self.configuration().changed.connect(self.reload)
        self._check.stateChanged.connect(self._toggleEnabled)

    def reload(self):
        self._name.setText(self.configuration().name())
        filename = self.configuration().thumbnail()
        if filename is None:
            pixmap = QtGui.QIcon(':icons/image.svg').pixmap(64, 64)
        else:
            pixmap = QtGui.QPixmap(filename)
            pixmap = pixmap.scaled(QtCore.QSize(64, 64), QtCore.Qt.KeepAspectRatio, QtCore.Qt.SmoothTransformation)
        self._thumbnail.setPixmap(pixmap)
        self._check.setCheckState(QtCore.Qt.Checked if self.configuration().enabled() else QtCore.Qt.Unchecked)

    def _toggleEnabled(self, state):
        cmd = commands.SetConfigurationEnabledCommand(configuration=self.configuration(), enabled=(state == QtCore.Qt.Checked))
        self.history().run(cmd)


class WorkspaceView(MainWindowMixin, WorkspaceMixin, QtWidgets.QWidget):
    selectionChanged = QtCore.pyqtSignal()

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.configurations().rowsInserted.connect(self._addRows)
        self.configurations().rowsRemoved.connect(self._removeRows)

        self._tree = QtWidgets.QTreeWidget(self)
        self._tree.setHeaderHidden(True)
        self._tree.itemSelectionChanged.connect(self._selectionChanged)
        self._tree.setAlternatingRowColors(True)

        self._properties = QtWidgets.QStackedWidget(self)
        label = QtWidgets.QLabel(_('Select a configuration'))
        label.setAlignment(QtCore.Qt.AlignCenter|QtCore.Qt.AlignHCenter)
        self._properties.addWidget(label)

        btnAdd = AddConfigurationButton(self, workspace=self.workspace(), mainWindow=self.mainWindow())
        btnDel = DeleteConfigurationsButton(self, workspace=self.workspace(), container=self, mainWindow=self.mainWindow())

        bld = LayoutBuilder(self)
        with bld.split() as self._splitter:
            with bld.vbox() as vbox:
                with bld.hbox() as header:
                    header.addStretch(1)
                    header.addWidget(btnDel)
                    header.addWidget(btnAdd)
                vbox.addWidget(self._tree, stretch=1)
            self._splitter.addWidget(self._properties)

    def loadState(self, settings):
        if settings.contains('MainSplitter'):
            self._splitter.restoreState(settings.value('MainSplitter'))
        else:
            self._splitter.setSizes([162, 471])

    def saveState(self, settings):
        settings.setValue('MainSplitter', self._splitter.saveState())

    def selection(self):
        return [item.data(0, QtCore.Qt.UserRole) for item in self._tree.selectedItems()]

    def _addRows(self, parent, first, last): # pylint: disable=W0613
        hasSelection = len(self._tree.selectedItems()) != 0
        for index, configuration in enumerate(self.configurations().items()[first:last+1]):
            item = QtWidgets.QTreeWidgetItem()
            item.setData(0, QtCore.Qt.UserRole, configuration)
            item.setFlags(QtCore.Qt.ItemIsSelectable|QtCore.Qt.ItemIsEnabled)
            self._tree.insertTopLevelItem(first + index, item)

            widget = ConfigurationItemWidget(self, mainWindow=self.mainWindow(), configuration=configuration)
            self._tree.setItemWidget(item, 0, widget)

            widget = ConfigurationView(self, mainWindow=self.mainWindow(), configuration=configuration)
            self._properties.insertWidget(first + index + 1, widget)

            if configuration.enabled() and not hasSelection:
                item.setSelected(True)
                hasSelection = True

    def _removeRows(self, parent, first, last): # pylint: disable=W0613
        for _ in range(last - first + 1):
            widget = self._properties.widget(first + 1)
            self._properties.removeWidget(widget)
            widget.deleteLater()

            self._tree.takeTopLevelItem(first)
        self._selectionChanged()

    def _selectionChanged(self):
        count = len(self.selection())
        if count == 1:
            configuration, = self.selection()
            index = self.configurations().indexOf(configuration)
            self._properties.setCurrentIndex(index + 1)
        else:
            self._properties.setCurrentIndex(0)

        self.selectionChanged.emit()
