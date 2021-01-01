#!/usr/bin/env python3

import os
import json
import tempfile
import logging

from PyQt5 import QtWidgets

from dsrlib.meta import Meta
from dsrlib.domain.downloader import StringDownloader, FileDownloader, AbortedError, DownloadError
from dsrlib.ui.mixins import MainWindowMixin
from dsrlib.ui.utils import LayoutBuilder


class ResourceDownloader(MainWindowMixin, QtWidgets.QDialog):
    logger = logging.getLogger('dsremap.resdl')

    STATE_MANIFEST = 0
    STATE_DL = 1
    STATE_ERROR = 2

    def __init__(self, parent, *, path, **kwargs):
        super().__init__(parent, **kwargs)
        self._path = path
        self._state = self.STATE_MANIFEST
        self._manifest = Meta.manifest()
        self._downloader = None

        self._msg = QtWidgets.QLabel(self)
        self._progress = QtWidgets.QProgressBar(self)

        bld = LayoutBuilder(self)
        with bld.vbox() as vbox:
            vbox.setContentsMargins(5, 5, 5, 5)
            vbox.addWidget(self._msg)
            vbox.addWidget(self._progress)
            with bld.hbox() as hbox:
                self._btn = QtWidgets.QPushButton(_('Cancel'), self)
                hbox.addStretch(1)
                hbox.addWidget(self._btn)

        self._btn.clicked.connect(self._cancel)

    def _cancel(self):
        if self._state == self.STATE_ERROR:
            self.reject()
        else:
            self._downloader.abort()

    def exec_(self): # pylint: disable=C0103
        self._msg.setText(_('Downloading manifest...'))
        self._downloader = StringDownloader(self, self.mainWindow().manager(), callback=self._gotManifest)
        self._downloader.downloadSize.connect(self._onSize)
        self._downloader.downloadProgress.connect(self._onProgress)
        self._downloader.get(Meta.imagesUrl().format(filename='manifest.json'))
        return super().exec_()

    def _onSize(self, size):
        self._progress.setMaximum(size)

    def _onProgress(self, value):
        self._progress.setValue(value)

    def _gotManifest(self, downloader):
        downloader.downloadSize.disconnect(self._onSize)
        downloader.downloadProgress.disconnect(self._onProgress)

        try:
            unused, data = downloader.result()
        except AbortedError:
            self.logger.info('Aborted manifest')
            if self._getValue(self._manifest, self._path + ('current', 'version')) != 0:
                self.accept()
            else:
                self.reject()
            return
        except DownloadError as exc:
            self.logger.error('Downloading manifest: %s', exc)
            if self._getValue(self._manifest, self._path + ('current', 'version')) != 0:
                self.accept()
            else:
                self._msg.setText(_('Cannot download manifest:{error}').format(error='<br /><font color="red">%s</font>' % str(exc)))
                self._btn.setText(_('Bummer'))
                self._state = self.STATE_ERROR
            return

        data = json.loads(data)
        self._getValue(self._manifest, self._path)['latest'] = self._getValue(data, self._path + ('latest',))

        if self._getValue(self._manifest, self._path + ('current', 'version')) == self._getValue(self._manifest, self._path + ('latest', 'version')):
            self.logger.info('Up to date')
            self.accept()
            return

        handle, filename = tempfile.mkstemp(dir=Meta.dataPath('images'))
        os.close(handle)

        self._state = self.STATE_DL
        self._msg.setText(_('Downloading image...'))
        self._downloader = FileDownloader(self, self.mainWindow().manager(), filename, callback=self._gotFile)
        self._downloader.downloadSize.connect(self._onSize)
        self._downloader.downloadProgress.connect(self._onProgress)
        self._downloader.get(Meta.imagesUrl().format(filename=self._getValue(self._manifest, self._path + ('latest', 'name'))))

    def _gotFile(self, downloader):
        downloader.downloadSize.disconnect(self._onSize)
        downloader.downloadProgress.disconnect(self._onProgress)

        try:
            downloader.result()
        except AbortedError:
            self.logger.info('Aborted image')
            if self._getValue(self._manifest, self._path + ('current', 'version')) != 0:
                self.accept()
            else:
                self.reject()
            return
        except DownloadError as exc:
            self.logger.error('Downloading image: %s', exc)
            if self._getValue(self._manifest, self._path + ('current', 'version')) != 0:
                self.accept()
            else:
                self._msg.setText(_('Cannot download image: {error}').format(error='<font color="red">%s</font>' % str(exc)))
                self._btn.setText(_('Bummer'))
                self._state = self.STATE_ERROR
            return

        src = downloader.filename()
        dst = os.path.join(Meta.dataPath('images'), self._getValue(self._manifest, self._path + ('latest', 'name')))
        if os.path.exists(dst):
            os.remove(dst)
        os.rename(src, dst)

        self._getValue(self._manifest, self._path)['current'] = self._getValue(self._manifest, self._path).pop('latest')
        Meta.updateManifest(self._manifest)
        self.accept()

    @staticmethod
    def _getValue(manifest, path):
        value = manifest
        for arg in path:
            value = value[arg]
        return value
