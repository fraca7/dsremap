#!/usr/bin/env python3

from PyQt5 import QtWidgets

from dsrlib.ui.mixins import MainWindowMixin


class Page(MainWindowMixin, QtWidgets.QWizardPage):
    def abort(self):
        pass
