#!/usr/bin/env python3

from PyQt5 import QtCore, QtWidgets

from dsrlib.meta import Meta
from dsrlib.ui.utils import LayoutBuilder


class AboutDialog(QtWidgets.QDialog):
    def __init__(self, parent):
        super().__init__(parent)

        self.setWindowTitle(_('{appname} v{appversion}').format(appname=Meta.appName(), appversion=str(Meta.appVersion())))

        iodev = QtCore.QFile(':/about.html')
        iodev.open(iodev.ReadOnly)
        try:
            about = bytes(iodev.readAll()).decode('utf-8')
        finally:
            iodev.close()
        about = about.format(author=Meta.appAuthor())

        text = QtWidgets.QTextBrowser(self)
        text.setHtml(about)
        text.setOpenExternalLinks(True)

        btn = QtWidgets.QPushButton(_('Done'), self)

        bld = LayoutBuilder(self)
        with bld.vbox() as vbox:
            vbox.addWidget(text)
            with bld.hbox() as buttons:
                buttons.addStretch(1)
                buttons.addWidget(btn)

        btn.clicked.connect(self.accept)

        self.resize(800, 600)
