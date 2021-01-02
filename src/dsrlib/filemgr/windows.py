#!/usr/bin/env python3

import os

from .base import FileManager


@FileManager.register
class FileManagerWindows:
    @staticmethod
    def showFile(filename):
        code = os.system('explorer.exe /select,"%s"' % filename)
        return code is None or code == 0
