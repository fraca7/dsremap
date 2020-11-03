#!/usr/bin/env python3

import os
import shutil
import math

from PyQt5 import QtCore, QtGui, QtWidgets

from dsrlib.domain import ConfigurationMixin, commands
from dsrlib.meta import Meta

from .actions import ActionWidgetBuilder
from .mixins import MainWindowMixin
from .utils import LayoutBuilder
from .uicommands import AddActionButton, DeleteActionsButton, ConvertToCustomActionButton


class ConfigurationSizeView(ConfigurationMixin, QtWidgets.QWidget):
    def __init__(self, parent, *, container, **kwargs):
        super().__init__(parent, **kwargs)
        self._container = container
        self.actions().rowsInserted.connect(self._onModelChange)
        self.actions().rowsRemoved.connect(self._onModelChange)
        self.actions().dataChanged.connect(self._onModelChange)
        self.setFixedWidth(200)

        self._container.selectionChanged.connect(self.update)

    def _onModelChange(self, *args): # pylint: disable=W0613
        self.update()

    def paintEvent(self, event): # pylint: disable=R0914,W0613
        painter = QtGui.QPainter(self)
        painter.setRenderHint(painter.Antialiasing, True)
        selection = set(self._container.selection())

        bsize = self.configuration().bytecodeSize()
        text = _('{count} bytes').format(count=bsize)

        font = painter.font()
        font.setPointSize(16)
        metrics = QtGui.QFontMetrics(font)
        textrc = metrics.boundingRect(text).adjusted(0, -5, 0, 5)
        height = textrc.height()

        rect = self.rect()
        painter.setFont(font)
        painter.setBackground(QtCore.Qt.white)
        painter.eraseRect(rect)
        painter.drawText(QtCore.QRect(0, 0, self.width(), height), QtCore.Qt.AlignCenter|QtCore.Qt.AlignHCenter, text)
        rect.setTopLeft(QtCore.QPoint(0, height))

        font.setPointSize(8)
        painter.setFont(font)
        metrics = QtGui.QFontMetrics(font)

        fgcolor, bgcolor = Meta.warningColors() if bsize > Meta.maxBytecodeSize() else Meta.standardColors()
        # For the purpose of displaying actions sizes, we remove the configuration size header
        bsize -= 2
        if bsize != 0:
            rect = QtCore.QRectF(rect).adjusted(5, 5, -5, -5)
            path = QtGui.QPainterPath()
            path.addRect(rect)
            painter.fillPath(path, bgcolor)
            painter.strokePath(path, bgcolor.darker())
            rect.adjust(1, 1, -1, -1)

            y = 0
            for index, action in enumerate(self.actions()):
                if index == len(self.actions()) - 1:
                    h = rect.height() - y
                else:
                    h = int(math.floor(action.size() / bsize * rect.height()))

                bbox = QtCore.QRectF(rect.x(), rect.y() + y, rect.width(), h)
                path = QtGui.QPainterPath()
                path.addRect(bbox)
                color = fgcolor.darker() if action in selection else fgcolor
                painter.fillPath(path, color)
                painter.strokePath(path, color.darker())

                text = action.label()
                text = metrics.elidedText(text, QtCore.Qt.ElideMiddle, bbox.width() - 2, 0)
                painter.drawText(bbox, QtCore.Qt.AlignCenter|QtCore.Qt.AlignHCenter, text)

                y += h


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

        self._size = ConfigurationSizeView(self, container=self, configuration=self.configuration())

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
                content.addWidget(self._size)

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
