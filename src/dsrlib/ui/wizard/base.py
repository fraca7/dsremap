#!/usr/bin/env python3

from PyQt5 import QtCore, QtGui, QtWidgets

from dsrlib.settings import Settings
from dsrlib.ui.mixins import MainWindowMixin


class Wizard(MainWindowMixin, QtWidgets.QWizard):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.setWizardStyle(self.MacStyle)
        self.setupPages()

        icon = QtGui.QIcon(':icons/gamepad.svg')
        self.setPixmap(self.BackgroundPixmap, icon.pixmap(256, 256))

        maxW, maxH = 0, 0
        for pageId in self.pageIds():
            page = self.page(pageId)
            size = page.sizeHint()
            maxW = max(maxW, size.width())
            maxH = max(maxH, size.height())
        for pageId in self.pageIds():
            page = self.page(pageId)
            page.setFixedSize(QtCore.QSize(maxW, maxH))

        # What. The. Fuck. First page too small on Linux.
        QtCore.QTimer.singleShot(0, self.adjustSize)

    def addPage(self, page):
        self.setPage(page.ID, page)

    def setupPages(self):
        pass

    def onSuccess(self):
        Settings().setFirmwareUploaded()

    def reject(self):
        self.currentPage().abort()
        super().reject()


class ManifestWizardMixin:
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._manifest = {}

    def setManifest(self, manifest):
        self._manifest = manifest

    def manifest(self):
        return self._manifest
