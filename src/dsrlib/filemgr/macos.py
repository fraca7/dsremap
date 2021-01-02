#!/usr/bin/env python3

import os

from .base import FileManager


@FileManager.register
class FileManagerMacOS:
    @staticmethod
    def showFile(filename):
        code = os.system('open -R "%s"' % filename)
        return code is None or code == 0
