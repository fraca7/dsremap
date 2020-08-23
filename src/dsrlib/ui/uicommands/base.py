#!/usr/bin/env python3

from PyQt5 import QtGui

import pyqtcmd

from dsrlib.ui.mixins import MainWindowMixin


class UICommand(MainWindowMixin, pyqtcmd.UICommand):
    def __init__(self, *args, **kwargs):
        if 'icon' in kwargs:
            kwargs['icon'] = QtGui.QIcon(':icons/%s.svg' % kwargs['icon'])
        if 'shortcut' in kwargs:
            kwargs['shortcut'] = QtGui.QKeySequence.fromString(kwargs['shortcut'])
        super().__init__(*args, **kwargs)

    def do(self): # make pylint happy
        raise NotImplementedError


class UndoUICommand(pyqtcmd.UndoUICommandMixin, UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, text=_('Undo'), icon='undo', shortcut='Ctrl+Z', tip=_('Undo last action'), **kwargs)


class RedoUICommand(pyqtcmd.RedoUICommandMixin, UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, text=_('Redo'), icon='redo', shortcut='Ctrl+R', tip=_('Redo last undone action'), **kwargs)
