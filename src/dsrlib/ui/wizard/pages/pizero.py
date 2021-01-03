#!/usr/bin/env python3

import os
import json
import binascii
import struct
import zipfile

from PyQt5 import QtCore, QtGui, QtWidgets

from dsrlib.domain.downloader import StringDownloader, DownloadError, DownloadNetworkError, DownloadHTTPError, AbortedError
from dsrlib.domain.device import DeviceVisitor
from dsrlib.ui.utils import LayoutBuilder
from dsrlib.filemgr import FileManager
from dsrlib.meta import Meta

from .pageids import PageId
from .base import Page


class PiZeroWifiPage(Page):
    ID = PageId.PiZeroWifi

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        bld = LayoutBuilder(self)
        with bld.vbox() as layout:
            msg = QtWidgets.QLabel(_('Please enter the credentials for your Wifi network.\nThe Raspberry Pi will automatically connect to this network.\nIf left empty, you will have to configure it yourself after creating the SD card.'), self)
            layout.addWidget(msg)

            with bld.form() as form:
                self._ssid = QtWidgets.QLineEdit(self)
                self._pwd1 = QtWidgets.QLineEdit(self)
                self._pwd2 = QtWidgets.QLineEdit(self)
                for pwd in (self._pwd1, self._pwd2):
                    pwd.setEchoMode(pwd.Password)
                self._ssh = QtWidgets.QCheckBox(_('Enable SSH'), self)

                form.addRow(_('SSID'), self._ssid)
                form.addRow(_('Password'), self._pwd1)
                form.addRow(_('Confirm password'), self._pwd2)
                form.addRow(self._ssh)

                for widget in (self._ssid, self._pwd1, self._pwd2):
                    widget.textChanged.connect(self._checkComplete)

            layout.addStretch(1)

    def initializePage(self):
        self.setTitle(_('Wifi configuration'))
        super().initializePage()

    def validatePage(self):
        self.wizard().setWifi(self._ssid.text() or None, self._pwd1.text() or None)
        self.wizard().setSsh(self._ssh.isChecked())
        return True

    def _checkComplete(self, _):
        self.completeChanged.emit()

    def isComplete(self):
        if self._pwd1.text() != '' or self._pwd2.text() != '' or self._ssid.text() != '':
            return self._pwd1.text() != '' and self._pwd1.text() == self._pwd2.text()
        return True

    def nextId(self):
        return PiZeroCopyPage.ID


class ImageCopyThread(QtCore.QThread):
    progress = QtCore.pyqtSignal(int, int)
    finished = QtCore.pyqtSignal()
    error = QtCore.pyqtSignal(object)

    def __init__(self, src, dst, ssid, password, ssh):
        self._src = src
        self._dst = dst
        self._ssid = None if ssid is None else ssid.encode('utf-8')
        self._password = None if password is None else password.encode('utf-8')
        self._ssh = ssh
        self._cancel = False
        super().__init__()

    def run(self):
        class Cancelled(Exception):
            pass

        with zipfile.ZipFile(self._src) as zipobj:
            info = zipobj.getinfo(os.path.basename(self._dst))
            size = info.file_size
            done = 0
            self.progress.emit(done, size)

            try:
                with zipobj.open(info) as src:
                    with open(self._dst, 'wb') as dst:
                        data = src.read(4096)
                        while data:
                            done += len(data)
                            self.progress.emit(done, size)
                            dst.write(data)
                            if self._cancel:
                                raise Cancelled()
                            data = src.read(4096)
                        self.progress.emit(size, size)

                        if self._ssid is not None and self._password is not None:
                            payload = b'%s%s%s' % (self._password, self._ssid, struct.pack('<IIBH', len(self._password), len(self._ssid), 1 if self._ssh else 0, 0xCAFE))
                            dst.write(b'\x00' * (512 - len(payload)))
                            dst.write(payload)
            except Cancelled:
                os.remove(self._dst)
            except Exception as exc: # pylint: disable=W0703
                self.error.emit(exc)
            else:
                self.finished.emit()

    def cancel(self):
        self._cancel = True


class PiZeroCopyPage(Page):
    ID = PageId.PiZeroCopy

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._finished = False
        self._thread = None

        bld = LayoutBuilder(self)
        with bld.vbox() as layout:
            layout.addStretch(1)
            self._progress = QtWidgets.QProgressBar(self)
            layout.addWidget(self._progress)
            layout.addStretch(1)

        self.setCommitPage(True)

    def initializePage(self):
        self.setTitle(_('Image copy'))
        self.setSubTitle(_('Copying image file'))

        manifest = Meta.manifest()
        name = manifest['rpi0w']['image']['current']['name']
        self._src = os.path.join(Meta.dataPath('images'), name)
        self._dst = os.path.join(QtCore.QStandardPaths.writableLocation(QtCore.QStandardPaths.DownloadLocation), '%s.img' % os.path.splitext(name)[0])

        ssid, password = self.wizard().wifi()
        self._thread = ImageCopyThread(self._src, self._dst, ssid, password, self.wizard().ssh())
        self._thread.progress.connect(self._onProgress)
        self._thread.finished.connect(self._onFinished)
        self._thread.error.connect(self._onError)
        self._thread.start()

    def cleanupPage(self):
        self.abort()

    def abort(self):
        if self._thread is not None:
            self._thread.cancel()
            self._thread.wait()
            self._thread = None

    def isComplete(self):
        return self._finished

    def nextId(self):
        return PiZeroBurnPage.ID

    def _onProgress(self, done, size):
        self._progress.setMaximum(size)
        self._progress.setValue(done)

    def _onFinished(self):
        self._thread.wait()
        self._thread = None
        self._finished = True
        self.completeChanged.emit()

        FileManager.showFile(self._dst)
        QtCore.QTimer.singleShot(0, self.wizard().button(self.wizard().NextButton).click)

    def _onError(self, exc):
        self._thread.wait()
        self._thread = None
        QtWidgets.QMessageBox.critical(self, _('Error'), _('Error copying file: {error}').format(error=str(exc)))
        self._finished = True
        self.setFinalPage(True)
        self.completeChanged.emit()


class PiZeroBurnPage(Page):
    ID = PageId.PiZeroBurn

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        bld = LayoutBuilder(self)
        with bld.vbox() as layout:
            layout.addStretch(1)
            msg = QtWidgets.QLabel(_('''
<p>The image file has been copied to your Downloads folder.</p>
<p>You can now use the <a href="https://www.raspberrypi.org/software/">Raspberry Pi Imager</a> to create the SD card for your Raspberry Pi Zero W, using the Custom image option.</p>
<p>Once you are done, insert the SD card in your Pi and power it on. Position it near your PS4 because you will have to connect both during the pairing process.</p>
<p>The first boot may take some time; wait until the device appears in the Device menu and then proceed with the pairing option.</p>
'''), self) # pylint: disable=C0301
            msg.setWordWrap(True)
            msg.setTextFormat(QtCore.Qt.RichText)
            msg.setOpenExternalLinks(True)
            layout.addWidget(msg)
            layout.addStretch(1)

    def initializePage(self):
        self.setTitle(_('Create SD card'))
        self.wizard().onSuccess()


class PiZeroPlugPage(Page):
    ID = PageId.PiZeroPlug

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        bld = LayoutBuilder(self)
        with bld.vbox() as layout:
            img = QtWidgets.QLabel(self)
            img.setPixmap(QtGui.QPixmap(':images/rpi_ps4.jpg'))
            img.setAlignment(QtCore.Qt.AlignCenter|QtCore.Qt.AlignHCenter)
            layout.addWidget(img)

    def initializePage(self):
        self.setTitle(_('Preparing to pair the PS4'))
        self.setSubTitle(_('First, power on your PS4 and plug the Raspberry Pi to it. Click Next when done.'))

    def nextId(self):
        return PiZeroPairHostPage.ID


class PiZeroPairHostPage(Page):
    ID = PageId.PiZeroPairHost

    STATE_CONTACTING = 0
    STATE_WAITING = 1
    STATE_PAIRING = 2
    STATE_ERROR = 3
    STATE_DONE = 4

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

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
        return PiZeroWaitDSPage.ID

    def _setState(self, state):
        self._state = state
        self.completeChanged.emit()
        self.setFinalPage(state == self.STATE_ERROR)
        if state == self.STATE_DONE:
            self.wizard().button(self.wizard().NextButton).click()

    def initializePage(self):
        self.setTitle(_('Pairing PS4'))

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


class PiZeroWaitDSPage(Page):
    ID = PageId.PiZeroWaitDS

    def __init__(self, *args, enumerator, **kwargs):
        super().__init__(*args, **kwargs)
        self._enumerator = enumerator

        bld = LayoutBuilder(self)
        with bld.vbox() as layout:
            img = QtWidgets.QLabel(self)
            img.setPixmap(QtGui.QPixmap(':images/dualshock_pc.jpg'))
            img.setAlignment(QtCore.Qt.AlignCenter|QtCore.Qt.AlignHCenter)
            layout.addWidget(img)

    def initializePage(self):
        self.setTitle(_('Waiting for Dualshock'))
        self.setSubTitle(_('You can now unplug the Raspberry Pi from the PS4, but leave it powered. Please plug your DualShock controller to this PC.'))

        self._found = False
        self._enumerator.connect(self)

    def cleanupPage(self):
        self._enumerator.disconnect(self)

    def isComplete(self):
        return self._found

    def nextId(self):
        return PiZeroPairDSPage.ID

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


class PiZeroPairDSPage(Page):
    ID = PageId.PiZeroPairDS

    STATE_REPORT = 0
    STATE_NETWORK = 1
    STATE_PAIR = 2
    STATE_ERROR = 3
    STATE_DONE = 4

    def initializePage(self):
        self.setTitle(_('Pairing controller'))
        self.setSubTitle(_('Pairing the controller, please wait...'))

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

    def isComplete(self):
        return self._state in (self.STATE_DONE, self.STATE_ERROR)

    def nextId(self):
        if self._state == self.STATE_ERROR:
            return -1
        return PiZeroFinalPage.ID

    def _setState(self, state):
        self._state = state
        self.completeChanged.emit()
        self.setFinalPage(state == self.STATE_ERROR)
        if state == self.STATE_DONE:
            self.wizard().button(self.wizard().NextButton).click()

    def _onReport(self, data):
        macaddr = ':'.join(['%02X' % val for val in reversed(data[1:7])])

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


class PiZeroFinalPage(Page):
    ID = PageId.PiZeroFinal

    def initializePage(self):
        self.setTitle(_('Finished'))
        self.setSubTitle(_('Done. You can unplug the DualShock. Next upload a configuration to the device through the Upload menu and you can start using your controller.'))
        self.wizard().onSuccess()
