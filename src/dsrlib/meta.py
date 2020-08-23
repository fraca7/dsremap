#!/usr/bin/env python3

import os
import sys
import collections
import platform
import glob
import tempfile

from PyQt5 import QtCore, QtGui
from serial.tools import list_ports


class Version(collections.namedtuple('Version', ['major', 'minor', 'patch'])):
    def __str__(self):
        return '%d.%d.%d' % self


class Meta:
    @staticmethod
    def appVersion():
        return Version(0, 99, 0)

    @staticmethod
    def firmwareVersion():
        return Version(0, 99, 0)

    @staticmethod
    def appName():
        return 'dsremap'

    @staticmethod
    def appDomain():
        return 'jeromelaheurte.net'

    @staticmethod
    def appAuthor():
        return 'J\u00e9r\u00f4me Laheurte'

    @staticmethod
    def appSite():
        # FIXME
        return 'https://www.example.com/'

    @staticmethod
    def dataPath(*args):
        path = QtCore.QStandardPaths.writableLocation(QtCore.QStandardPaths.AppDataLocation)
        path = os.path.join(path, *args)
        if not os.path.exists(path):
            os.makedirs(path)
        return path

    @staticmethod
    def newThumbnail(filename):
        _, ext = os.path.splitext(filename)
        handle, path = tempfile.mkstemp(suffix=ext, dir=Meta.dataPath('thumbnails'))
        os.close(handle)
        os.remove(path)
        return path

    @staticmethod
    def maxBytecodeSize():
        return 1024 # Leonardo EEPROM size

    @staticmethod
    def standardColors():
        return (QtGui.QColor(0xDC, 0xED, 0xC1), QtGui.QColor(0xFF, 0xFF, 0xFF))

    @staticmethod
    def warningColors():
        return (QtGui.QColor(0xFF, 0xAA, 0xA5), QtGui.QColor(0xFF, 0xAA, 0xA5))

    @staticmethod
    def listSerials():
        if platform.system() == 'Darwin':
            return set(glob.glob('/dev/cu.usbmodem*'))
        if platform.system() == 'Linux':
            return set(glob.glob('/dev/ttyACM*'))
        if platform.system() == 'Windows':
            return {info.device for info in list_ports.comports()}
        raise RuntimeError('Unsupported platform')

    @staticmethod
    def isFrozen():
        if platform.system() == 'Linux':
            return os.path.basename(sys.argv[0]) == 'dsremap'
        return hasattr(sys, 'frozen') and sys.frozen # pylint: disable=E1101

    @staticmethod
    def avrdudeConf():
        if Meta.isFrozen():
            if platform.system() == 'Darwin':
                return os.path.normpath(os.path.join(os.path.dirname(sys.executable), '..', 'Resources', 'avrdude.conf'))
            if platform.system() == 'Linux':
                return os.path.join(os.environ['APPDIR'], 'data', 'avrdude.conf')
            if platform.system() == 'Windows':
                return os.path.normpath(os.path.join(os.path.dirname(sys.executable), 'resources', 'avrdude.conf'))
            raise RuntimeError('Unsupported platform')
        return os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..', 'res', 'avrdude.conf'))

    @staticmethod
    def firmware():
        if Meta.isFrozen():
            if platform.system() == 'Darwin':
                return os.path.normpath(os.path.join(os.path.dirname(sys.executable), '..', 'Resources', 'dsremap.ino.hex'))
            if platform.system() == 'Linux':
                return os.path.join(os.environ['APPDIR'], 'data', 'dsremap.ino.hex')
            if platform.system() == 'Windows':
                return os.path.normpath(os.path.join(os.path.dirname(sys.executable), 'resources', 'dsremap.ino.hex'))
            raise RuntimeError('Unsupported platform')
        return os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..', 'arduino-build', 'dsremap.ino.hex'))

    @staticmethod
    def defaultConfigurations():
        if Meta.isFrozen():
            if platform.system() == 'Darwin':
                return os.path.normpath(os.path.join(os.path.dirname(sys.executable), '..', 'Resources', 'configurations'))
            if platform.system() == 'Linux':
                return os.path.join(os.environ['APPDIR'], 'data', 'configurations')
            if platform.system() == 'Windows':
                return os.path.normpath(os.path.join(os.path.dirname(sys.executable), 'resources', 'configurations'))
            raise RuntimeError('Unsupported platform')
        return os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..', 'res', 'configurations'))

    @staticmethod
    def messages():
        if Meta.isFrozen():
            if platform.system() == 'Darwin':
                return os.path.normpath(os.path.join(os.path.dirname(sys.executable), '..', 'Resources', 'i18n'))
            if platform.system() == 'Linux':
                return os.path.join(os.environ['APPDIR'], 'data', 'i18n')
            if platform.system() == 'Windows':
                return os.path.normpath(os.path.join(os.path.dirname(sys.executable), 'resources', 'i18n'))
            raise RuntimeError('Unsupported platform')
        return os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..', 'i18n'))

    @staticmethod
    def decodePlatformString(pstring):
        return QtCore.QTextCodec.codecForLocale().toUnicode(pstring)
