#!/usr/bin/env python3

import os
import shutil

from PyQt5 import QtCore, QtGui, QtWidgets

from dsrlib.domain import ConfigurationMixin, commands
from dsrlib.meta import Meta

from .actions import ActionWidgetBuilder
from .mixins import MainWindowMixin
from .utils import LayoutBuilder
from .uicommands import AddActionButton, DeleteActionsButton, ConvertToCustomActionButton


class ThumbnailView(MainWindowMixin, ConfigurationMixin, QtWidgets.QWidget):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.setFixedWidth(170)
        self.setFixedHeight(300)
        self._pixmap = None
        self.reload()
        self.setAcceptDrops(True)

    def reload(self):
        filename = self.configuration().thumbnail()
        if filename is None:
            self._pixmap = QtGui.QIcon(':icons/image.svg').pixmap(150, 150)
        else:
            pixmap = QtGui.QPixmap(filename)
            self._pixmap = pixmap.scaled(QtCore.QSize(150, 300), QtCore.Qt.KeepAspectRatio, QtCore.Qt.SmoothTransformation)
        self.update()

    def paintEvent(self, event): # pylint: disable=W0613
        painter = QtGui.QPainter(self)
        painter.setRenderHint(painter.Antialiasing, True)
        rect = QtCore.QRect(QtCore.QPoint(0, 0), self._pixmap.size())
        rect.moveCenter(self.rect().center())
        painter.drawPixmap(rect.topLeft(), self._pixmap)

        if self.configuration().thumbnail() is None:
            text = _('Drop an image here')
            bbox = painter.fontMetrics().boundingRect(text)
            bbox.moveCenter(rect.center())
            bbox.moveTop(rect.bottom())
            painter.drawText(bbox, 0, text)

    def dragEnterEvent(self, event):
        data = event.mimeData()
        if data.hasFormat('text/uri-list'):
            if len(data.urls()) != 1:
                return
            url = data.urls()[0]
            if not url.isLocalFile():
                return
            filename = url.toLocalFile()
            if not os.path.isfile(filename):
                return

            pixmap = QtGui.QPixmap(filename)
            if pixmap.isNull():
                return

            event.setDropAction(QtCore.Qt.CopyAction)
            event.accept()

    def dropEvent(self, event):
        src = event.mimeData().urls()[0].toLocalFile()
        dst = Meta.newThumbnail(src)
        shutil.copyfile(src, dst)
        cmd = commands.ChangeConfigurationThumbnailCommand(configuration=self.configuration(), filename=dst)
        self.history().run(cmd)


class ConfigurationView(MainWindowMixin, ConfigurationMixin, QtWidgets.QWidget):
    selectionChanged = QtCore.pyqtSignal()

    def __init__(self, parent, **kwargs):
        super().__init__(parent, **kwargs)

        self.actions().rowsInserted.connect(self._addRows)
        self.actions().rowsRemoved.connect(self._removeRows)

        self._name = QtWidgets.QLineEdit(self.configuration().name(), self)
        self._name.editingFinished.connect(self._changeName)
        self._thumbnail = ThumbnailView(self, configuration=self.configuration(), mainWindow=self.mainWindow())

        # We don't actually use a QTreeView because setItemWidget is
        # convenient. Not in the mood to write a custom delegate.
        self._tree = QtWidgets.QTreeWidget(self)
        self._tree.setHeaderHidden(True)
        self._tree.itemSelectionChanged.connect(self.selectionChanged)
        self._tree.setAlternatingRowColors(True)

        btnAdd = AddActionButton(self, configuration=self.configuration(), mainWindow=self.mainWindow())
        btnDel = DeleteActionsButton(self, configuration=self.configuration(), container=self, mainWindow=self.mainWindow())
        btnConv = ConvertToCustomActionButton(self, configuration=self.configuration(), container=self, mainWindow=self.mainWindow())

        bld = LayoutBuilder(self)
        with bld.vbox():
            with bld.hbox() as header:
                header.addWidget(self._name)
                header.addWidget(btnConv)
                header.addWidget(btnDel)
                header.addWidget(btnAdd)
            with bld.hbox() as content:
                content.addWidget(self._thumbnail)
                content.addWidget(self._tree, stretch=1)

        self.configuration().changed.connect(self._updateValues)

        # In case of redo, the model may not be empty
        count = len(self.actions())
        if count:
            self._addRows(None, 0, count - 1)

    def selection(self):
        return [item.data(0, QtCore.Qt.UserRole) for item in self._tree.selectedItems()]

    def _addRows(self, parent, first, last): # pylint: disable=W0613
        for index, action in enumerate(self.actions().items()[first:last+1]):
            item = QtWidgets.QTreeWidgetItem()
            item.setData(0, QtCore.Qt.UserRole, action)
            self._tree.insertTopLevelItem(first + index, item)

            widget = ActionWidgetBuilder(self, self.mainWindow()).visit(action)
            self._tree.setItemWidget(item, 0, widget)
            widget.geometryChanged.connect(item.emitDataChanged)

    def _removeRows(self, parent, first, last): # pylint: disable=W0613
        for _ in range(last - first + 1):
            self._tree.takeTopLevelItem(first)

    def _changeName(self):
        name = self._name.text()
        if name != self.configuration().name():
            cmd = commands.ChangeConfigurationNameCommand(configuration=self.configuration(), name=name)
            self.history().run(cmd)

    def _updateValues(self):
        self._name.setText(self.configuration().name())
        self._thumbnail.reload()
