#!/usr/bin/env python3

from PyQt5 import QtCore


class ListObjectProxy:
    def __init__(self, obj, model):
        self._obj = obj
        self._model = model
        obj.changed.connect(self._notify)

    def object(self):
        return self._obj

    def _notify(self):
        self._model.notifyChange(self)

    def shutdown(self):
        self._obj.changed.disconnect(self._notify)
        obj, self._obj = self._obj, None
        self._model = None
        return obj


class ListModel(QtCore.QAbstractItemModel):
    def __init__(self, parent=None, columns=1):
        super().__init__(parent)
        self._columns = columns
        self._items = []

    def __iter__(self):
        return [proxy.object() for proxy in self._items].__iter__()

    def __len__(self):
        return len(self._items)

    def __getitem__(self, row):
        return self._items[row].object()

    def __setitem__(self, row, value):
        self._items[row].shutdown()
        self._items[row] = ListObjectProxy(value, self)
        self.dataChanged.emit(self.index(row, 0, QtCore.QModelIndex()), self.index(row, self.columnCount(QtCore.QModelIndex()) - 1, QtCore.QModelIndex()))

    def indexOf(self, item):
        for index, proxy in enumerate(self._items):
            if proxy.object() == item:
                return index
        raise IndexError()

    def items(self):
        return [proxy.object() for proxy in self._items]

    def clear(self):
        self.beginResetModel()
        self._clear()
        self.endResetModel()

    def _clear(self):
        for proxy in self._items:
            proxy.shutdown()
        self._items = []

    def addItem(self, item, index=None):
        if index is None:
            index = len(self._items)
        self.beginInsertRows(QtCore.QModelIndex(), index, index)
        self._items.insert(index, ListObjectProxy(item, self))
        self.endInsertRows()

    def removeItem(self, item):
        index = self.indexOf(item)
        self.beginRemoveRows(QtCore.QModelIndex(), index, index)
        self._items[index].shutdown()
        self._items.pop(index)
        self.endRemoveRows()
        return index

    def columnCount(self, parentIndex): # pylint: disable=W0613
        return self._columns

    def rowCount(self, parentIndex):
        if parentIndex.isValid():
            return 0
        return len(self._items)

    def index(self, row, col, parentIndex): # pylint: disable=W0613
        return self.createIndex(row, col, self._items[row])

    def parent(self, index): # pylint: disable=W0613
        return QtCore.QModelIndex()

    def data(self, index, role):
        item = self._items[index.row()].object()
        if role == QtCore.Qt.UserRole:
            return item
        return None

    def notifyChange(self, proxy):
        row = self._items.index(proxy)
        self.dataChanged.emit(self.index(row, 0, QtCore.QModelIndex()), self.index(row, self.columnCount(QtCore.QModelIndex()) - 1, QtCore.QModelIndex()))
