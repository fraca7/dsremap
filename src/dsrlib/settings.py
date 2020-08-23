#!/usr/bin/env python3

import os
import platform
import contextlib

from PyQt5 import QtCore


class Settings(QtCore.QSettings):
    @contextlib.contextmanager
    def grouped(self, *names):
        self.beginGroup('/'.join(names))
        try:
            yield self
        finally:
            self.endGroup()

    def avrdude(self):
        with self.grouped('Settings', 'Misc'):
            filename = self.value('avrdude', '')
        if filename and os.path.exists(filename):
            return filename

        search = os.environ['PATH'].split(os.pathsep)
        if platform.system() == 'Darwin':
            # When frozen the PATH is empty
            search.extend(['/usr/bin', '/usr/local/bin'])
        for path in search:
            filename = os.path.join(path, 'avrdude.exe' if platform.system() == 'Windows' else 'avrdude')
            if os.path.exists(filename):
                return filename

        if platform.system() == 'Windows': # pylint: disable=R1702
            # To my knowledge, avrdude only comes in 32 bits flavor
            import winreg # pylint: disable=C0415,E0401
            try:
                key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r'SOFTWARE\WinAVR', 0, winreg.KEY_WOW64_32KEY|winreg.KEY_READ)
                try:
                    index = 0
                    while True:
                        try:
                            name, data, _ = winreg.EnumValue(key, index)
                            if name.isdigit() and isinstance(data, str):
                                filename = os.path.join(data, 'bin', 'avrdude.exe')
                                if os.path.exists(filename):
                                    return filename
                        except WindowsError: # pylint: disable=E0602
                            break
                        index += 1
                finally:
                    winreg.CloseKey(key)
            except WindowsError: # pylint: disable=E0602
                pass

        return None

    def setAvrdude(self, filename):
        with self.grouped('Settings', 'Misc'):
            self.setValue('avrdude', filename)

    def firmwareUploaded(self):
        with self.grouped('Settings'):
            return self.booleanValue('firmwareUploaded', False)

    def setFirmwareUploaded(self):
        with self.grouped('Settings'):
            self.setBooleanValue('firmwareUploaded', True)

    # Depending on the platform, some versions of QSettings fuck up the types

    def setBooleanValue(self, name, value):
        self.setValue(name, int(value))

    def booleanValue(self, name, default=False):
        return bool(int(self.value(name, default)))
