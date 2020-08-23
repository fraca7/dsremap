#!/usr/bin/env python3

from PyQt5 import QtCore, QtGui, QtWidgets

from dsrlib.domain.changelog import HTMLChangelogFormatter
from dsrlib.meta import Meta
from dsrlib.ui.utils import LayoutBuilder


class ChangelogView(QtWidgets.QDialog):
    def __init__(self, parent, changelog):
        super().__init__(parent)

        self.setWindowTitle(_('New release available'))

        view = QtWidgets.QTextBrowser(self)
        view.setHtml(HTMLChangelogFormatter().format(changelog.changesSince(Meta.appVersion())))
        view.setOpenExternalLinks(True)

        btn = QtWidgets.QPushButton(_('OK'), self)
        btn.clicked.connect(self.accept)

        bld = LayoutBuilder(self)
        with bld.vbox() as vbox:
            vbox.addWidget(view)
            with bld.hbox() as hbox:
                hbox.addStretch(1)
                hbox.addWidget(btn)

        self.resize(640, 480)

    def accept(self):
        QtGui.QDesktopServices.openUrl(QtCore.QUrl(Meta.releasesUrl()))
        return super().accept()
