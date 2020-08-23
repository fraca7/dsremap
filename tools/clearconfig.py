#!/usr/bin/env python3

import os
import sys
import shutil

from PyQt5 import QtCore

sys.path.insert(0, os.path.normpath(os.path.join(os.path.dirname(__file__), '..', 'src')))

from dsrlib.ui.main import Application
from dsrlib.meta import Meta

if __name__ == '__main__':
    app = Application([])
    settings = QtCore.QSettings()
    settings.clear()
    settings.sync()
    shutil.rmtree(Meta.dataPath())
