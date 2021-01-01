#!/usr/bin/env python3

import os
import logging

from PyQt5 import QtCore, QtGui


class FileManager:
    logger = logging.getLogger('dsremap.filemgr')

    @classmethod
    def showFile(cls, filename):
        code = os.system('open -R "%s"' % filename)
        if code:
            cls.logger.error('open returned code %s', code)
            QtGui.QDesktopServices.openUrl(QtCore.QUrl.fromLocalFile(os.path.dirname(filename)))
