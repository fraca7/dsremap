#!/usr/bin/env python3

from PyQt5 import QtCore
import pydbus

from .base import FileManager


class FileManagerDBus:
    bus = pydbus.SessionBus()


@FileManager.register
class FileManagerFreedesktop(FileManagerDBus):
    @classmethod
    def showFile(cls, filename):
        url = QtCore.QUrl.fromLocalFile(filename)
        mgr = cls.bus.get('org.freedesktop.FileManager1', '/org/freedesktop/FileManager1')
        mgr.ShowItems([url.toString()], '')
        return True
