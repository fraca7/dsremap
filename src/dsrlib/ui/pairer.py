#!/usr/bin/env python3

import binascii
import json

from PyQt5 import QtCore, QtGui, QtWidgets

from dsrlib.ui.mixins import MainWindowMixin
from dsrlib.ui.utils import LayoutBuilder
from dsrlib.domain.device import DeviceVisitor
from dsrlib.domain.downloader import StringDownloader, DownloadError, DownloadNetworkError, DownloadHTTPError, AbortedError


class PairHostWaitPage(QtWidgets.QWizardPage):
    def __init__(self, parent):
        super().__init__(parent)

        self.setTitle(_('Preparing to pair the PS4'))
        self.setSubTitle(_('First, power on your PS4 and plug the Raspberry Pi to it. Click Next when done.'))

        bld = LayoutBuilder(self)
        with bld.vbox() as layout:
            img = QtWidgets.QLabel(self)
            img.setPixmap(QtGui.QPixmap(':images/rpi_ps4.jpg'))
            img.setAlignment(QtCore.Qt.AlignCenter|QtCore.Qt.AlignHCenter)
            layout.addWidget(img)


class PairHostPage(MainWindowMixin, QtWidgets.QWizardPage):
    STATE_CONTACTING = 0
    STATE_WAITING = 1
    STATE_PAIRING = 2
    STATE_ERROR = 3
    STATE_DONE = 4

    def __init__(self, parent, **kwargs):
        super().__init__(parent, **kwargs)
        self.setTitle(_('Pairing PS4'))

        bld = LayoutBuilder(self)
        with bld.vbox() as layout:
            self._message = QtWidgets.QTextEdit(self)
            self._message.setReadOnly(True)
            layout.addWidget(self._message)

    def isComplete(self):
        return self._state in (self.STATE_DONE, self.STATE_ERROR)

    def nextId(self):
        # Without this the Next button is still shown and enabled even after calling setFinalPage(True)
        if self._state == self.STATE_ERROR:
            return -1
        return super().nextId()

    def _setState(self, state):
        self._state = state
        self.completeChanged.emit()
        self.setFinalPage(state == self.STATE_ERROR)
        if state == self.STATE_DONE:
            self.wizard().button(self.wizard().NextButton).click()

    def initializePage(self):
        self._setState(self.STATE_CONTACTING)
        self._message.hide()
        self._downloader = None
        self._retryCount = 0
        self._timer = QtCore.QTimer(self)
        self._timer.setSingleShot(True)
        self._timer.timeout.connect(self._getInfo)
        self._getInfo()

    def cleanupPage(self):
        if self._downloader is not None:
            self._downloader.abort()
        if self._state == self.STATE_WAITING:
            self._timer.stop()

    def _getInfo(self):
        self._setState(self.STATE_CONTACTING)
        self.setSubTitle(_('Getting device information...'))

        def gotInfo(downloader):
            self._downloader = None
            try:
                _unused, text = downloader.result()
            except AbortedError:
                self._setState(self.STATE_ERROR)
                return
            except DownloadNetworkError as exc:
                self._retryCount += 1
                if self._retryCount == 10:
                    self.setSubTitle(_('Error connecting to the device: {error}').format(error=str(exc)))
                    self._setState(self.STATE_ERROR)
                    return

                self._setState(self.STATE_WAITING)
                self._timer.start(5000)
                return
            except DownloadHTTPError as exc:
                self.setSubTitle(_('Error getting info from device: {error}').format(error=str(exc)))
                self._setState(self.STATE_ERROR)
                return

            data = json.loads(text)
            self.wizard().setDongle(data['bt_interfaces'][0])
            self._pair()

        dev = self.wizard().device()
        self._downloader = StringDownloader(self, self.mainWindow().manager(), callback=gotInfo)
        self._downloader.get('http://%s:%d/info' % (dev.addr, dev.port))

    def _pair(self):
        self._setState(self.STATE_PAIRING)
        self.setSubTitle(_('Waiting for the PS4...'))

        def pairingDone(downloader):
            self._downloader = None
            try:
                _unused, text = downloader.result()
            except AbortedError:
                return
            except DownloadError as exc:
                self.setSubTitle(_('Error while pairing the PS4: {code}').format(code=str(exc)))
                self._setState(self.STATE_ERROR)
                return

            data = json.loads(text)
            if data['status'] == 'ko':
                self.setSubTitle(_('Error while pairing the PS4'))
                self._message.setPlainText(data['stderr'])
                self._setState(self.STATE_ERROR)
                return

            self.wizard().setLinkKey(data['key'])
            self._setState(self.STATE_DONE)

        dev = self.wizard().device()

        url = QtCore.QUrl('http://%s:%d/setup_ps4' % (dev.addr, dev.port))
        query = QtCore.QUrlQuery()
        query.addQueryItem('interface', self.wizard().dongle())
        url.setQuery(query)
        self._downloader = StringDownloader(self, self.mainWindow().manager(), callback=pairingDone)
        self._downloader.get(url)


class WaitDualshockPage(QtWidgets.QWizardPage):
    def __init__(self, parent, enumerator):
        super().__init__(parent)
        self._enumerator = enumerator

        self.setTitle(_('Waiting for Dualshock'))
        self.setSubTitle(_('You can now unplug the Raspberry Pi from the PS4, but leave it powered. Please plug your DualShock controller to this PC.'))

        bld = LayoutBuilder(self)
        with bld.vbox() as layout:
            img = QtWidgets.QLabel(self)
            img.setPixmap(QtGui.QPixmap(':images/dualshock_pc.jpg'))
            img.setAlignment(QtCore.Qt.AlignCenter|QtCore.Qt.AlignHCenter)
            layout.addWidget(img)

    def initializePage(self):
        self._found = False
        self._enumerator.connect(self)

    def cleanupPage(self):
        self._enumerator.disconnect(self)

    def onDeviceAdded(self, device):
        class Visitor(DeviceVisitor):
            def __init__(self, listener):
                self.listener = listener

            def acceptArduino(self, dev):
                pass

            def acceptNetworkDevice(self, dev):
                pass

            def acceptDualshock(self, dev):
                self.listener.onDualshockAdded(dev)

        Visitor(self).visit(device)

    def onDualshockAdded(self, device):
        self._found = True
        self.wizard().setDualshock(device)
        self.completeChanged.emit()
        self.wizard().button(self.wizard().NextButton).click()

    def onDeviceRemoved(self, dev):
        pass

    def isComplete(self):
        return self._found


class PairControllerPage(MainWindowMixin, QtWidgets.QWizardPage):
    STATE_REPORT = 0
    STATE_NETWORK = 1
    STATE_PAIR = 2
    STATE_ERROR = 3
    STATE_DONE = 4

    def __init__(self, parent, **kwargs):
        super().__init__(parent, **kwargs)

        self.setTitle(_('Pairing controller'))
        self.setSubTitle(_('Pairing the controller, please wait...'))

    def _setState(self, state):
        self._state = state
        self.completeChanged.emit()
        self.setFinalPage(state == self.STATE_ERROR)
        if state == self.STATE_DONE:
            self.wizard().button(self.wizard().NextButton).click()

    def isComplete(self):
        return self._state in (self.STATE_DONE, self.STATE_ERROR)

    def nextId(self):
        if self._state == self.STATE_ERROR:
            return -1
        return super().nextId()

    def initializePage(self):
        self._setState(self.STATE_REPORT)
        self._downloader = None

        self.setSubTitle(_('Obtaining MAC address...'))
        self._worker = self.wizard().dualshock().worker()
        self._worker.start()
        self._worker.reportReceived.connect(self._onReport)
        self._worker.getReport(0x81, 64)

    def cleanupPage(self):
        if self._downloader is not None:
            self._downloader.abort()

    def _onReport(self, data):
        macaddr = ':'.join(['%02X' % val for val in reversed(data[1:])])

        self._setState(self.STATE_NETWORK)
        self.setSubTitle(_('Uploading pairing info...'))

        def gotResponse(downloader):
            self._downloader = None
            try:
                _unused, text = downloader.result()
            except AbortedError:
                self._setState(self.STATE_ERROR)
                return
            except DownloadNetworkError as exc:
                self.setSubTitle(_('Error connecting to the device: {error}').format(error=str(exc)))
                self._setState(self.STATE_ERROR)
                return
            except DownloadHTTPError as exc:
                self.setSubTitle(_('Error uploading pairing info: {error}').format(error=str(exc)))
                self._setState(self.STATE_ERROR)
                return

            data = json.loads(text)
            self.setSubTitle(_('Pairing the controller...'))
            self._setState(self.STATE_PAIR)

            report = b'\x13' + bytes(reversed(binascii.unhexlify(data['interface'].replace(':', '')))) + binascii.unhexlify(data['link_key'])
            self._worker.write(report)
            self._worker.cancel() # Actually this is blocking but shouldn't take long...

            self._setState(self.STATE_DONE)

        dev = self.wizard().device()
        url = QtCore.QUrl('http://%s:%d/setup_ds4' % (dev.addr, dev.port))
        query = QtCore.QUrlQuery()
        query.addQueryItem('interface', self.wizard().dongle())
        query.addQueryItem('ds4', macaddr)
        query.addQueryItem('link_key', self.wizard().linkKey())
        url.setQuery(query)
        self._downloader = StringDownloader(self, self.mainWindow().manager(), callback=gotResponse)
        self._downloader.get(url)


class FinalPage(QtWidgets.QWizardPage):
    def __init__(self, parent):
        super().__init__(parent)
        self.setTitle(_('Finished'))
        self.setSubTitle(_('Done. You can unplug the DualShock and press PS to start using it through the proxy.'))


class PairingWizard(MainWindowMixin, QtWidgets.QWizard):
    def __init__(self, parent, device, enumerator, **kwargs):
        super().__init__(parent, **kwargs)
        self._device = device
        self._dualshock = None
        self._linkkey = None
        self._dongle = None

        self.setWizardStyle(self.MacStyle)

        self.addPage(PairHostWaitPage(self))
        self.addPage(PairHostPage(self, mainWindow=self.mainWindow()))
        self.addPage(WaitDualshockPage(self, enumerator))
        self.addPage(PairControllerPage(self, mainWindow=self.mainWindow()))
        self.addPage(FinalPage(self))

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

    def device(self):
        return self._device

    def setDualshock(self, dev):
        self._dualshock = dev

    def dualshock(self):
        return self._dualshock

    def setLinkKey(self, key):
        self._linkkey = key

    def linkKey(self):
        return self._linkkey

    def setDongle(self, addr):
        self._dongle = addr

    def dongle(self):
        return self._dongle
