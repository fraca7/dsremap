#!/usr/bin/env python3

import logging
import queue
import struct
import time

from PyQt5 import QtCore
import hid

from dsrlib.meta import Version


class LocalDevice:
    def __init__(self, path, name, vid, pid):
        self.path = path
        self.name = name
        self.vid = vid
        self.pid = pid

    def worker(self):
        return HIDDeviceWorker(self)


class Dualshock(LocalDevice):
    pass


class Arduino(LocalDevice):
    def __init__(self, path, name, vid, pid, fwVersion):
        super().__init__(path, name, vid, pid)
        self.fwVersion = fwVersion


class HIDDeviceWorker(QtCore.QThread):
    reportReceived = QtCore.pyqtSignal(bytes)

    def __init__(self, device):
        super().__init__()

        self._device = device
        self._queue = queue.Queue()

    def cancel(self):
        self._queue.put((None, None, None))
        self.wait()

    def run(self):
        handle = hid.device()
        handle.open(self._device.vid, self._device.pid)

        while True:
            rid, length, data = self._queue.get()
            if rid is None:
                break
            if rid == 0:
                handle.send_feature_report(data)
            else:
                data = handle.get_feature_report(rid, length)
                self.reportReceived.emit(bytes(data))

        handle.close()

    def write(self, data):
        self._queue.put((0, 0, data))

    def getReport(self, reportId, length):
        self._queue.put((reportId, length, None))



class HIDEnumerator(QtCore.QThread):
    deviceAdded = QtCore.pyqtSignal(LocalDevice)
    deviceRemoved = QtCore.pyqtSignal(LocalDevice)

    logger = logging.getLogger('dsremap.HIDEnumerator')

    def __init__(self):
        super().__init__()

        self._devices = dict()
        self._stop = False

    def stop(self):
        self._stop = True
        self.wait()

    def connect(self, target):
        for dev in self._devices.values():
            target.onDeviceAdded(dev)
        self.deviceAdded.connect(target.onDeviceAdded)
        self.deviceRemoved.connect(target.onDeviceRemoved)

    def disconnect(self, target):
        self.deviceAdded.disconnect(target.onDeviceAdded)
        self.deviceRemoved.disconnect(target.onDeviceRemoved)
        for dev in self._devices.values():
            target.onDeviceRemoved(dev)

    def run(self):
        while not self._stop:
            found = set()
            for device in hid.enumerate():
                found.add(device['path'])
                if device['path'] not in self._devices and device['vendor_id'] == 0x54c:
                    vid, pid = device['vendor_id'], device['product_id']
                    self.logger.info('Found device %04x/%04x', vid, pid)
                    handle = hid.device()
                    try:
                        try:
                            handle.open(vid, pid)
                            data = bytes(handle.get_feature_report(0x80, 7))
                            # Looks like on Windows we get "aligned" buffers (length 8 here)
                            _, major, minor, patch = struct.unpack('<BHHH', data[:7])
                            dev = Arduino(device['path'], device['product_string'], vid, pid, Version(major, minor, patch))
                        finally:
                            handle.close()
                    except: # pylint: disable=W0702
                        self.logger.info('Device %04x/%04x does not answer report 0x80', vid, pid)
                        dev = Dualshock(device['path'], device['product_string'], vid, pid)

                    self._devices[device['path']] = dev
                    self.deviceAdded.emit(dev)

            for path, dev in list(self._devices.items()):
                if path not in found:
                    self.logger.info('Device %04x/%04x unplugged', dev.vid, dev.pid)
                    del self._devices[path]
                    self.deviceRemoved.emit(dev)

            time.sleep(1)
