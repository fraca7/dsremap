#!/usr/bin/env python3

from PyQt5 import QtWidgets, Qsci

from dsrlib.settings import Settings
from dsrlib.ui.utils import LayoutBuilder


class Editor(QtWidgets.QDialog):
    def __init__(self, *args, action, **kwargs):
        self._action = action
        super().__init__(*args, **kwargs)

        self._sci = Qsci.QsciScintilla(self)
        self._sci.setUtf8(True)
        self._sci.setEolMode(self._sci.EolUnix)
        self._sci.setIndentationsUseTabs(True)
        self._sci.setIndentationGuides(True)
        self._sci.setCaretLineVisible(True)
        self._sci.setMargins(1)
        self._sci.setMarginType(0, self._sci.NumberMargin)
        self._sci.setMarginWidth(0, '000')
        self._sci.setLexer(Qsci.QsciLexerCPP(self._sci))
        self._sci.setText(self._action.source())

        btnOK = QtWidgets.QPushButton(_('Done'), self)
        btnCancel = QtWidgets.QPushButton(_('Cancel'), self)

        bld = LayoutBuilder(self)
        with bld.vbox() as vbox:
            vbox.addWidget(self._sci)
            with bld.hbox() as buttons:
                buttons.addStretch(1)
                buttons.addWidget(btnCancel)
                buttons.addWidget(btnOK)

        btnOK.clicked.connect(self.accept)
        btnCancel.clicked.connect(self.reject)

        with Settings().grouped('IDE') as settings:
            if settings.contains('WindowGeometry'):
                self.restoreGeometry(settings.value('WindowGeometry'))
            else:
                self.resize(1024, 768)

    def accept(self):
        with Settings().grouped('IDE') as settings:
            settings.setValue('WindowGeometry', self.saveGeometry())
        return super().accept()

    def reject(self):
        with Settings().grouped('IDE') as settings:
            settings.setValue('WindowGeometry', self.saveGeometry())
        return super().reject()

    def source(self):
        return self._sci.text()
