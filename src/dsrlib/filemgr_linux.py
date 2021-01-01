#!/usr/bin/env python3

import os
import logging

from PyQt5 import QtCore, QtGui

import pydbus


class FileManager:
    logger = logging.getLogger('dsremap.filemgr')

    @classmethod
    def showFile(cls, filename):
        url = QtCore.QUrl.fromLocalFile(filename)
        try:
            bus = pydbus.SessionBus()
            mgr = bus.get('org.freedesktop.FileManager1', '/org/freedesktop/FileManager1')
            mgr.ShowItems([url.toString()], 'dsremap') # WTF is the StartupId ?
        except Exception as exc: # pylint: disable=W0703
            cls.logger.error('DBus error: %s', exc)
            QtGui.QDesktopServices.openUrl(QtCore.QUrl.fromLocalFile(os.path.dirname(filename)))
