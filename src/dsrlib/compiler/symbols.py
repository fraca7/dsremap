#!/usr/bin/env python3

import enum
import collections


class Symbol(collections.namedtuple('Symbol', ['name', 'value', 'size'])):
    def __str__(self):
        return self.name


@enum.unique
class SymbolScope(enum.Enum):
    GLOBAL = 'global'
    LOCAL = 'local'
    THIS = 'this'
    ARGS = 'args'
    INHERITED = 'inherited'


class SymbolTable:
    def __init__(self, *, parent=None):
        self._symbols = []
        self._size = 0
        self._parent = parent
        self._children = []
        if parent is None:
            self._type = SymbolScope.GLOBAL
        else:
            self._type = SymbolScope.INHERITED
            parent.addChild(self)

    def addSize(self, delta):
        self._size += delta

    def parent(self):
        return self._parent

    def offset(self, name):
        offset = 0
        for symbol in self._symbols:
            if symbol.name == name:
                return offset
            offset += symbol.size
        return 0 # for pylint

    def addChild(self, child):
        self._children.append(child)

    def dump(self, indent=0):
        print('%s%s (%d)' % ('  ' * indent, self._type.name, self._size))
        for symbol in self._symbols:
            print('%s  %s (%d)' % ('  ' * indent, symbol.name, symbol.size))
        for child in self._children:
            child.dump(indent=indent + 1)

    def __iter__(self):
        return self._symbols.__iter__()

    def type(self):
        return self._type

    def setType(self, type_):
        self._type = type_

    def size(self):
        return self._size

    def has(self, name, recurse=True):
        for symbol in self._symbols:
            if symbol.name == name:
                return True

        if recurse and self._parent is not None:
            return self._parent.has(name)

        return False

    def add(self, name, value, size=None):
        size = value.type().size if size is None else size
        self._size += size
        self._symbols.append(Symbol(name, value, size))

    def get(self, name):
        for symbol in self._symbols:
            if symbol.name == name:
                return symbol
        if self._parent is not None:
            return self._parent.get(name)
        raise KeyError(name)
