#!/usr/bin/env python3

import os
import tempfile
import codecs
import json

from PyQt5 import QtWidgets

from dsrlib.ui.utils import LayoutBuilder
from dsrlib.domain.downloader import FileDownloader, DownloadNetworkError, DownloadHTTPError, AbortedError
from dsrlib.meta import Meta

from .pageids import PageId
from .base import Page


class DeviceChoicePage(Page):
    ID = PageId.DeviceType

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._nextId = -1

        bld = LayoutBuilder(self)
        with bld.vbox() as layout:
            layout.addStretch(1)
            self._addButton(layout, _('Arduino Leonardo'), PageId.ArduinoAvrdude, True)
            self._addButton(layout, _('Raspberry Pi Zero W'), PageId.PiZeroManifestDownload)
            self._addButton(layout, _('Do that later'), DeviceNotConfiguredPage.ID)
            layout.addStretch(1)

    def _addButton(self, layout, label, pageId, checked=False):
        btn = QtWidgets.QRadioButton(label, self)
        btn.setChecked(checked)
        if checked:
            self._nextId = pageId

        def setNextId():
            self._nextId = pageId
        btn.clicked.connect(setNextId)
        layout.addWidget(btn)

    def initializePage(self):
        self.setTitle(_('Device type'))
        self.setSubTitle(_('Choose what kind of device you want to configure'))

    def nextId(self):
        return self._nextId


class DeviceNotConfiguredPage(Page):
    ID = PageId.DeviceNotConfigured

    def initializePage(self):
        self.setTitle(_('Skipped'))
        self.setSubTitle(_('Skipped initial device configuration. You can configure your devices later from the Devices menu.'))
        self.wizard().onSuccess()

    def nextId(self):
        return -1


class DownloadFilePage(Page):
    STATE_DL = 0
    STATE_ERROR = 1
    STATE_FINISHED = 2

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        bld = LayoutBuilder(self)
        with bld.vbox() as layout:
            self._progress = QtWidgets.QProgressBar(self)
            layout.addStretch(1)
            layout.addWidget(self._progress)
            layout.addStretch(1)

        self._downloader = None
        self._state = self.STATE_DL

    def url(self):
        raise NotImplementedError

    def onNetworkError(self, exc): # pylint: disable=W0613
        raise NotImplementedError

    def onDownloadError(self, exc): # pylint: disable=W0613
        raise NotImplementedError

    def onDownloadFinished(self, filename):
        raise NotImplementedError

    def tempfile(self, **kwargs):
        handle, name = tempfile.mkstemp(**kwargs)
        os.close(handle)
        return name

    def initializePage(self):
        self._progress.show()
        self._download()

    def cleanupPage(self):
        if self._downloader is not None:
            self._downloader.abort()
            self._downloader = None

    def isComplete(self):
        return self._state == self.STATE_FINISHED

    def setState(self, state):
        self._state = state
        if state == self.STATE_ERROR:
            self._progress.hide()
            self.setFinalPage(True)
        if state == self.STATE_FINISHED:
            self.completeChanged.emit()
            self.wizard().button(self.wizard().NextButton).click()

    def _download(self):
        self._state = self.STATE_DL

        tempname = self.tempfile()

        def gotFile(downloader):
            self._downloader.downloadSize.disconnect(self._gotSize)
            self._downloader.downloadProgress.disconnect(self._gotProgress)
            self._downloader = None

            try:
                downloader.result()
            except AbortedError:
                self.setState(self.STATE_FINISHED)
            except DownloadNetworkError as exc:
                self.onNetworkError(exc)
            except DownloadHTTPError as exc:
                self.onDownloadError(exc)
            else:
                self.onDownloadFinished(tempname)
            finally:
                if os.path.exists(tempname):
                    os.remove(tempname)

        self._downloader = FileDownloader(self.wizard(), self.mainWindow().manager(), tempname, callback=gotFile)
        self._downloader.downloadSize.connect(self._gotSize)
        self._downloader.downloadProgress.connect(self._gotProgress)
        self._downloader.get(self.url())

    def _gotSize(self, size):
        self._progress.setMaximum(size)
        self._progress.setValue(0)

    def _gotProgress(self, progress):
        self._progress.setValue(progress)


class ManifestDownloadPage(DownloadFilePage):
    def url(self):
        return Meta.imagesUrl().format(filename='manifest.json')

    def currentVersion(self, manifest):
        raise NotImplementedError

    def onNetworkError(self, exc):
        self.setSubTitle(_('Unable to download manifest: {error}').format(error=str(exc)))
        self.setState(self.STATE_FINISHED if self._exists else self.STATE_ERROR)

    def onDownloadError(self, exc):
        self.setSubTitle(_('Error downloading manifest: {error}').format(error=str(exc)))
        self.setState(self.STATE_FINISHED if self._exists else self.STATE_ERROR)

    def onDownloadFinished(self, filename):
        with codecs.getreader('utf-8')(open(filename, 'rb')) as fileobj:
            manifest = json.load(fileobj)

        existing = Meta.manifest()
        existing['leonardo']['firmware']['latest'] = manifest['leonardo']['firmware']['latest']
        existing['rpi0w']['image']['latest'] = manifest['rpi0w']['image']['latest']
        existing['rpi0w']['server']['latest'] = manifest['rpi0w']['server']['latest']
        Meta.updateManifest(existing)

        self.setState(self.STATE_FINISHED)

    def initializePage(self):
        self.setSubTitle(_('Downloading manifest...'))
        self._exists = self.currentVersion(Meta.manifest()) != 0
        super().initializePage()
