#!/usr/bin/env python3

from PyQt5 import QtWidgets

from dsrlib.meta import Meta
from dsrlib.ui.utils import LayoutBuilder


class AboutDialog(QtWidgets.QDialog):
    def __init__(self, parent):
        super().__init__(parent)

        self.setWindowTitle(_('{appname} v{appversion}').format(appname=Meta.appName(), appversion=str(Meta.appVersion())))

        about = _('''
Copyright 2020 {author}

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Icons courtesy of <a href="https://iconmonstr.com/">iconmonstr</a>.

''').format(author=Meta.appAuthor()).replace('\n', '<br />')

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
