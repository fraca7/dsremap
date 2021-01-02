#!/usr/bin/env python3

import os
import logging

from PyQt5 import QtCore, QtGui


class FileManager:
    handlers = []

    logger = logging.getLogger('dsremap.filemgr')

    @classmethod
    def register(cls, handler):
        cls.handlers.append(handler)
        return handler

    @classmethod
    def showFile(cls, filename):
        filename = os.path.abspath(filename)
        for handler in reversed(cls.handlers):
            try:
                if handler.showFile(filename):
                    break
            except Exception as exc: # pylint: disable=W0703
                cls.logger.warning('Handler failed: %s', exc)
        else:
            cls.logger.error('No handler succeeded')


@FileManager.register
class FileManagerQt:
    @staticmethod
    def showFile(filename):
        QtGui.QDesktopServices.openUrl(QtCore.QUrl.fromLocalFile(os.path.dirname(filename)))
        return True
