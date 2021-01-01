#!/usr/bin/env python3

import os
import platform

from PyQt5 import QtCore, QtGui


if platform.system() == 'Linux':
    from .filemgr_linux import FileManager # pylint: disable=W0611
elif platform.system() == 'Darwin':
    from .filemgr_mac import FileManager # pylint: disable=W0611
else:
    class FileManager:
        @staticmethod
        def showFile(filename):
            QtGui.QDesktopServices.openUrl(QtCore.QUrl.fromLocalFile(os.path.dirname(filename)))
