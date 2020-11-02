#!/usr/bin/env python3

from PyQt5 import QtCore, QtGui, QtWidgets, Qsci

from ptk.parser import ParseError
from ptk.lexer import LexerError

from dsrlib.settings import Settings
from dsrlib.ui.utils import LayoutBuilder
from dsrlib.compiler.bcgen import generateBytecode, BytecodeGenError


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

        self._report = QtWidgets.QTreeWidget(self)
        self._report.setColumnCount(2)
        self._report.setHeaderHidden(True)
        self._report.itemClicked.connect(self._reportClicked)

        self._error_mk = self._sci.markerDefine(self._sci.Background)
        self._sci.setMarkerBackgroundColor(QtCore.Qt.red, self._error_mk)
        self._warn_mk = self._sci.markerDefine(self._sci.Background)
        self._sci.setMarkerBackgroundColor(QtCore.Qt.yellow, self._warn_mk)

        btnOK = QtWidgets.QPushButton(_('Done'), self)
        btnCancel = QtWidgets.QPushButton(_('Cancel'), self)

        bld = LayoutBuilder(self)
        with bld.vbox() as vbox:
            vbox.addWidget(self._sci, stretch=5)
            vbox.addWidget(self._report, stretch=1)
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

        self._timer = QtCore.QTimer(self)
        self._timer.timeout.connect(self._tryCompile)
        self._timer.setSingleShot(True)
        self._sci.textChanged.connect(self._onTextChanged)
        self._tryCompile()

    def _onTextChanged(self):
        self._timer.start(3000)

    def _tryCompile(self):
        self._sci.markerDeleteAll(self._error_mk)
        self._sci.clearAnnotations()
        self._report.hide()
        while self._report.topLevelItemCount():
            self._report.takeTopLevelItem(0)

        try:
            warnings, _ = generateBytecode(self.source())
        except (LexerError, ParseError) as exc:
            self._addError(exc.position.line, str(exc))
            self._report.show()
            return
        except BytecodeGenError as exc:
            for msg, pos in exc.errors:
                self._addError(pos.line, msg)
            self._report.show()
            return

        if warnings:
            for msg, pos in warnings:
                self._addWarning(pos.line, msg)
            self._report.show()

    def _reportClicked(self, item):
        line = item.data(0, QtCore.Qt.UserRole)
        self._sci.ensureLineVisible(line)

    def _addReport(self, line, msg, mk, icon):
        self._sci.markerAdd(line - 1, mk)
        self._sci.annotate(line - 1, msg, 0) # XXXFIXME style
        item = QtWidgets.QTreeWidgetItem(self._report, ['%d' % line, msg])
        item.setIcon(1, QtGui.QIcon(':icons/%s.svg' % icon))
        item.setData(0, QtCore.Qt.UserRole, line)

    def _addError(self, line, msg):
        self._addReport(line, msg, self._error_mk, 'error')

    def _addWarning(self, line, msg):
        self._addReport(line, msg, self._warn_mk, 'warning')

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
