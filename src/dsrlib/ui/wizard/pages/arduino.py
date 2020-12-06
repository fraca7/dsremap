#!/usr/bin/env python3

import platform
import logging
import time

import serial
from PyQt5 import QtGui, QtCore, QtWidgets

from dsrlib.settings import Settings
from dsrlib.meta import Meta
from dsrlib.ui.utils import LayoutBuilder

from .pageids import PageId
from .base import Page


class ArduinoWelcomePage(Page):
    ID = PageId.ArduinoWelcome

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        bld = LayoutBuilder(self)
        with bld.vbox() as layout:
            layout.addStretch(1)

    def initializePage(self):
        self.setTitle(_('Firmware upload'))
        self.setSubTitle(_('Welcome. The first thing to do is to program your Leonardo board. If you choose to skip this step, you can do it whenever using the Devices menu in the menu bar.'))

    def nextId(self):
        settings = Settings()
        if settings.avrdude() is None:
            return ArduinoAvrdudePage.ID
        return ArduinoResetPage.ID


class ArduinoAvrdudePage(Page):
    ID = PageId.ArduinoAvrdude

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        btnTry = QtWidgets.QPushButton(_('Try to detect avrdude again'), self)
        btnBrowse = QtWidgets.QPushButton(_('Browse'), self)
        self._msg = QtWidgets.QLabel(self)

        if platform.system() == 'Darwin':
            install = QtWidgets.QLabel(_('You can install <b>avrdude</b> on mac OS using <a href="https://brew.sh">Homebrew</a>. After installing brew, launch a terminal and type<br /><pre>brew install avrdude</pre>'), self)
        elif platform.system() == 'Linux':
            install = QtWidgets.QLabel(_('You can install <b>avrdude</b> on Linux using your regular package manager, for instance<br /><pre>apt-get install avrdude</pre>'), self)
        elif platform.system() == 'Windows':
            install = QtWidgets.QLabel(_('You can install <b>avrdude</b> on Windows using <a href="https://sourceforge.net/projects/winavr/">WinAVR</a>'), self)
        else:
            raise RuntimeError('Unsupported platform')
        install.setOpenExternalLinks(True)

        bld = LayoutBuilder(self)
        with bld.vbox() as layout:
            layout.addWidget(self._msg)
            layout.addWidget(btnTry)
            layout.addWidget(btnBrowse)
            layout.addWidget(install)
            layout.addStretch(1)

        btnTry.clicked.connect(self._tryAgain)
        btnBrowse.clicked.connect(self._browse)

    def initializePage(self):
        self.setTitle(_('Install avrdude'))
        self.setSubTitle(_('The <b>avrdude</b> program could not be found on the path. Please install it or manually specify its path here.'))

        self.wizard().setButtonText(self.wizard().CustomButton1, _('Cancel'))

    def isComplete(self):
        return Settings().avrdude() is not None

    def nextId(self):
        return ArduinoResetPage.ID

    def _tryAgain(self):
        self._msg.setText(_('avrdude found') if self.isComplete() else _('Cannot find avrdude'))
        self.completeChanged.emit()
        self.wizard().button(self.wizard().NextButton).click()

    def _browse(self):
        filename, dummy = QtWidgets.QFileDialog.getOpenFileName(self.parent(), _('Browse for avrdude'))
        if filename:
            Settings().setAvrdude(filename)
            self.completeChanged.emit()


class ArduinoResetPage(Page):
    ID = PageId.ArduinoReset

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        bld = LayoutBuilder(self)
        with bld.vbox() as layout:
            img = QtWidgets.QLabel(self)
            img.setPixmap(QtGui.QPixmap(':images/leonardo_reset.jpg'))
            img.setAlignment(QtCore.Qt.AlignCenter|QtCore.Qt.AlignHCenter)
            layout.addWidget(img)

    def initializePage(self):
        self.setTitle(_('Firmware upload'))
        self.setSubTitle(_('Please plug the Leonardo to this computer and press the <b>reset</b> button. Click Continue while holding it.'))

        self.wizard().setButtonText(self.wizard().CustomButton1, _('Cancel'))

    def nextId(self):
        return ArduinoFindSerialPage.ID


class ArduinoFindSerialPage(Page):
    ID = PageId.ArduinoFindSerial

    logger = logging.getLogger('avrdude')

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self._timer = QtCore.QTimer(self)
        self._timer.timeout.connect(self._tick)

        # It's ugly without an actual widget
        bld = LayoutBuilder(self)
        with bld.vbox() as layout:
            layout.addStretch(1)

    def initializePage(self):
        self.setTitle(_('Looking for Leonardo'))
        self.setSubTitle(_('Release the <b>reset</b> button now.'))

        self._state = 0
        self._finished = False
        self._devices = Meta.listSerials()
        self._devname = None
        self._process = None
        self._timer.start(200)

    def cleanupPage(self):
        self._timer.stop()
        if self._process is not None:
            self._process.terminate() # Err...

    def isComplete(self):
        return self._finished

    def _tick(self):
        if self._state == 0:
            newDevs = Meta.listSerials() - self._devices
            if newDevs:
                # XXXTODO: support multiple devices ? Would be hard since they disappear after a few seconds...
                self._devname = newDevs.pop()
                self.setSubTitle(_('Found Leonardo at {path}.').format(path=self._devname))
                self._state = 1
        if self._state == 1:
            # Reset
            error = None
            for unused in range(5):
                try:
                    with serial.Serial(self._devname, 2000):
                        pass
                except serial.SerialException as exc:
                    error = exc
                    time.sleep(0.5)
                    # Retry
                else:
                    break
            else:
                self.setSubTitle(_('Error resetting the device: {error}').format(error=str(error)))
                self._finished = True
                self._timer.stop()
                self.completeChanged.emit()
                return
            self._state = 2
        if self._state == 2:
            self._state = 3
            self._timer.stop()
            self._launchUpdate()

    def _launchUpdate(self):
        self.setTitle(_('Updating'))
        self.setSubTitle(_('Update in progress...'))

        self.wizard().button(self.wizard().CustomButton1).setEnabled(False)
        self.wizard().button(self.wizard().BackButton).setEnabled(False)

        self._process = QtCore.QProcess(self)
        self._process.errorOccurred.connect(self._onError)
        self._process.finished.connect(self._onFinished)
        self._process.readyReadStandardError.connect(self._onStderr)
        self._process.readyReadStandardOutput.connect(self._onStdout)

        args = [
            '-patmega32u4',
            '-cavr109',
            '-P%s' % self._devname,
            '-b57600',
            '-D',
            '-Uflash:w:%s:i' % Meta.firmware(),
            ]

        if platform.system() != 'Windows':
            args.insert(0, '-C%s' % Meta.avrdudeConf())

        self._process.start(Settings().avrdude(), args)

    def _onError(self, code):
        msg = {
            QtCore.QProcess.FailedToStart: _('Process failed to start'),
            QtCore.QProcess.Crashed: _('Process crashed'),
            QtCore.QProcess.Timedout: _('Timeout'),
            QtCore.QProcess.WriteError: _('Write error'),
            QtCore.QProcess.ReadError: _('Read error'),
            QtCore.QProcess.UnknownError: _('Unknown error'),
            }[code]
        self.logger.error('Process error: %s', msg)
        self.setSubTitle(_('Error launching avrdude: {error}').format(error=msg))
        self._process = None
        self._finished = True
        self.completeChanged.emit()

    def _onFinished(self, code, status):
        self.logger.info('avrdude exit: %d %d', code, status)

        if code == 0:
            self.setSubTitle(_('Done.'))
            self.wizard().onSuccess()
        else:
            self.setSubTitle(_('<b>avrdude</b> exited with code {code}. Something went wrong.').format(code=code))

        self._process = None
        self._finished = True
        self.completeChanged.emit()

    def _onStderr(self):
        text = Meta.decodePlatformString(self._process.readAllStandardError())
        self.logger.info('E: %s', text.strip())

    def _onStdout(self):
        text = Meta.decodePlatformString(self._process.readAllStandardOutput())
        self.logger.info('O: %s', text.strip())
