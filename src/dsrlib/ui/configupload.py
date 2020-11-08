#!/usr/bin/env python3

import zlib
import struct

from PyQt5 import QtCore, QtWidgets, QtNetwork

from dsrlib.domain.mixins import WorkspaceMixin
from dsrlib.ui.mixins import MainWindowMixin

from .utils import LayoutBuilder


class ConfigurationHIDUploader(WorkspaceMixin, QtWidgets.QDialog):
    def __init__(self, parent, *, device, **kwargs):
        super().__init__(parent, **kwargs)
        self._progress = QtWidgets.QProgressBar(self)
        self._bytecode = self.workspace().bytecode()
        self._offset = 0
        self._worker = device.worker()
        self._crc = zlib.crc32(self._bytecode)

        self._progress.setMinimum(0)
        self._progress.setMaximum(len(self._bytecode))
        self._progress.setValue(0)

        bld = LayoutBuilder(self)
        with bld.vbox() as layout:
            self._label = QtWidgets.QLabel(_('Uploading to {name}...').format(name=device.name), self)
            layout.addWidget(self._label)
            layout.addWidget(self._progress)
            layout.setContentsMargins(5, 5, 5, 5)

    def exec_(self): # pylint: disable=C0103
        self._worker.start()
        QtCore.QTimer.singleShot(0, self._next)
        return super().exec_()

    def _next(self):
        length = min(59, len(self._bytecode))
        data = struct.pack('<BHH', 0x88, self._offset, length) + self._bytecode[:length]
        self._bytecode = self._bytecode[length:]
        if len(data) < 64:
            data += b'\x00' * (64 - len(data))
        self._worker.write(data)

        self._offset += length
        self._progress.setValue(self._offset)

        if self._bytecode:
            QtCore.QTimer.singleShot(1000, self._next)
        else:
            QtCore.QTimer.singleShot(1000, self._check)

    def _check(self):
        self._worker.reportReceived.connect(self._gotReport)
        self._label.setText(_('CRC check...'))
        self._worker.getReport(0x84, 5)

    def _gotReport(self, data):
        unused, crc = struct.unpack('<BI', data[:5])
        if crc == self._crc:
            self.accept()
        else:
            QtWidgets.QMessageBox.warning(self, _('Warning'), _('The CRC check failed. You will probably want to retry.'))
            self.reject()
        self._worker.cancel()


class ConfigurationNetworkUploader(WorkspaceMixin, MainWindowMixin, QtWidgets.QDialog):
    def __init__(self, parent, *, device, **kwargs):
        super().__init__(parent, **kwargs)

        bld = LayoutBuilder(self)
        with bld.vbox() as layout:
            layout.addWidget(QtWidgets.QLabel(_('Uploading to {name}...').format(name=device.name), self))

        req = QtNetwork.QNetworkRequest(QtCore.QUrl('http://%s:%d/set_config' % (device.addr, device.port)))
        req.setHeader(req.ContentTypeHeader, 'application/octet-stream')
        self._reply = self.mainWindow().manager().post(req, self.workspace().bytecode())
        self._reply.finished.connect(self._onQueryResponse)

    def _onQueryResponse(self):
        status = self._reply.attribute(QtNetwork.QNetworkRequest.HttpStatusCodeAttribute)
        if status is None:
            QtWidgets.QMessageBox.critical(self, _('Upload error'), _('Cannot connect to device.'))
            self.reject()
        elif status != 200:
            QtWidgets.QMessageBox.critical(self, _('Upload error'), _('Device returned HTTP status {code}').format(code=status))
            self.reject()
        else:
            self.accept()
