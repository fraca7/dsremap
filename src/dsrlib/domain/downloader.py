#!/usr/bin/env python3

import os
import io
import enum

from PyQt5 import QtCore, QtNetwork


@enum.unique
class DownloadState(enum.Enum):
    Idle = enum.auto()
    Connecting = enum.auto()
    Uploading = enum.auto()
    Downloading = enum.auto()
    Finished = enum.auto()


class SSLError(Exception):
    def __init__(self, errors):
        super().__init__()
        self.errors = errors


class DownloadError(Exception):
    def __init__(self, code, msg):
        super().__init__(msg)
        self.code = code


class AbortedError(DownloadError):
    pass


class DownloadNetworkError(DownloadError):
    """Low-level errors (connection refused, closed, etc)"""


class DownloadHTTPError(DownloadError):
    """HTTP-level errors"""


class Downloader(QtCore.QObject): # pylint: disable=R0902
    stateChanged = QtCore.pyqtSignal(DownloadState)

    downloadSize = QtCore.pyqtSignal(int)
    downloadProgress = QtCore.pyqtSignal(int)

    uploadSize = QtCore.pyqtSignal(int)
    uploadProgress = QtCore.pyqtSignal(int)

    def __init__(self, parent, manager, *, callback=None):
        super().__init__(parent)
        self._manager = manager
        self._callback = callback
        self._state = DownloadState.Idle
        self._reply = None
        self._error = None
        self._status = None
        self._dsize = None
        self._usize = None

    def state(self):
        return self._state

    def result(self):
        if self._error is not None:
            cls, args = self._error
            raise cls(*args)
        return self._status

    def _request(self, req):
        if isinstance(req, str):
            req = QtCore.QUrl(req)
        if isinstance(req, QtCore.QUrl):
            req = QtNetwork.QNetworkRequest(req)
        return req

    def _setState(self, state):
        self._state = state
        self.stateChanged.emit(state)

    def _query(self, func, request, *args):
        self._reply = func(self._request(request), *args)
        self._reply.downloadProgress.connect(self._onDownloadProgress)
        self._reply.uploadProgress.connect(self._onUploadProgress)
        if QtCore.QT_VERSION >= 0x050f00:
            self._reply.errorOccurred.connect(self._onError)
        else:
            self._reply.error.connect(self._onError)
        self._reply.sslErrors.connect(self._onSSLError)
        self._reply.finished.connect(self._onRequestFinished)
        self._setState(DownloadState.Connecting)

    def get(self, request):
        self._query(self._manager.get, request)

    def postBytes(self, request, data):
        self._query(self._manager.post, request, data)

    def abort(self):
        self._reply.abort()

    def _onDownloadProgress(self, done, todo):
        if self._dsize is None:
            self._setState(DownloadState.Downloading)
            self._dsize = todo
            self.downloadSize.emit(todo)
        self.downloadProgress.emit(done)

    def _onUploadProgress(self, done, todo):
        if self._usize is None:
            self._setState(DownloadState.Uploading)
            self._usize = todo
            self.uploadSize.emit(todo)
        self.uploadProgress.emit(done)

    def _onError(self, error):
        cls = {
            QtNetwork.QNetworkReply.ConnectionRefusedError: DownloadNetworkError,
            QtNetwork.QNetworkReply.RemoteHostClosedError: DownloadNetworkError,
            QtNetwork.QNetworkReply.HostNotFoundError: DownloadNetworkError,
            QtNetwork.QNetworkReply.TimeoutError: DownloadNetworkError,
            QtNetwork.QNetworkReply.OperationCanceledError: AbortedError,
            QtNetwork.QNetworkReply.SslHandshakeFailedError: DownloadNetworkError,
            QtNetwork.QNetworkReply.TemporaryNetworkFailureError: DownloadNetworkError,
            QtNetwork.QNetworkReply.NetworkSessionFailedError: DownloadNetworkError,
            QtNetwork.QNetworkReply.BackgroundRequestNotAllowedError: DownloadNetworkError,
            QtNetwork.QNetworkReply.TooManyRedirectsError: DownloadHTTPError,
            QtNetwork.QNetworkReply.InsecureRedirectError: DownloadHTTPError,
            QtNetwork.QNetworkReply.ProxyConnectionRefusedError: DownloadNetworkError,
            QtNetwork.QNetworkReply.ProxyConnectionClosedError: DownloadNetworkError,
            QtNetwork.QNetworkReply.ProxyNotFoundError: DownloadNetworkError,
            QtNetwork.QNetworkReply.ProxyTimeoutError: DownloadNetworkError,
            QtNetwork.QNetworkReply.ProxyAuthenticationRequiredError: DownloadHTTPError,
            QtNetwork.QNetworkReply.ContentAccessDenied: DownloadHTTPError,
            QtNetwork.QNetworkReply.ContentOperationNotPermittedError: DownloadHTTPError,
            QtNetwork.QNetworkReply.ContentNotFoundError: DownloadHTTPError,
            QtNetwork.QNetworkReply.AuthenticationRequiredError: DownloadHTTPError,
            QtNetwork.QNetworkReply.ContentReSendError: DownloadHTTPError,
            QtNetwork.QNetworkReply.ContentConflictError: DownloadHTTPError,
            QtNetwork.QNetworkReply.ContentGoneError: DownloadHTTPError,
            QtNetwork.QNetworkReply.InternalServerError: DownloadHTTPError,
            QtNetwork.QNetworkReply.OperationNotImplementedError: DownloadHTTPError,
            QtNetwork.QNetworkReply.ServiceUnavailableError: DownloadHTTPError,
            QtNetwork.QNetworkReply.ProtocolUnknownError: DownloadHTTPError,
            QtNetwork.QNetworkReply.ProtocolInvalidOperationError: DownloadHTTPError,
            QtNetwork.QNetworkReply.UnknownNetworkError: DownloadNetworkError,
            QtNetwork.QNetworkReply.UnknownProxyError: DownloadNetworkError,
            QtNetwork.QNetworkReply.UnknownContentError: DownloadHTTPError,
            QtNetwork.QNetworkReply.ProtocolFailure: DownloadHTTPError,
            QtNetwork.QNetworkReply.UnknownServerError: DownloadNetworkError,
            }.get(error, DownloadNetworkError)
        self._error = (cls, (error, self._reply.errorString()))

    def _onSSLError(self, errors):
        self._error = (SSLError, (list(errors),))

    def _onRequestFinished(self):
        self._status = self._reply.attribute(QtNetwork.QNetworkRequest.HttpStatusCodeAttribute)
        self._setState(DownloadState.Finished)
        if self._callback is not None:
            self._callback(self)


class FileDownloader(Downloader):
    def __init__(self, parent, manager, filename, **kwargs):
        super().__init__(parent, manager, **kwargs)
        self._filename = filename

    def abort(self):
        super().abort()
        os.remove(self._filename)

    def filename(self):
        return self._filename

    def _query(self, func, request, *args):
        super()._query(func, request, *args)
        self._reply.readyRead.connect(self._onReadyRead)

    def _onReadyRead(self):
        with open(self._filename, 'a+b') as fileobj:
            fileobj.write(bytes(self._reply.read(self._reply.bytesAvailable())))


class StringDownloader(Downloader):
    def __init__(self, parent, manager, *, encoding='utf-8', **kwargs):
        super().__init__(parent, manager, **kwargs)
        self._encoding = encoding
        self._buffer = io.StringIO()

    def _query(self, func, request, *args):
        super()._query(func, request, *args)
        self._reply.readyRead.connect(self._onReadyRead)

    def result(self):
        status = super().result()
        return status, self._buffer.getvalue()

    def _onReadyRead(self):
        self._buffer.write(bytes(self._reply.read(self._reply.bytesAvailable())).decode(self._encoding))
