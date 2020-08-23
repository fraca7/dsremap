#!/usr/bin/env python3

import time
import queue
import logging
import struct

import hid

from PyQt5 import QtCore

from dsrlib.meta import Version


class HIDDevice:
    def __init__(self, path, name, vid, pid):
        super().__init__()

        self.path = path
        self.name = name
        self.vid = vid
        self.pid = pid
        self.fwVersion = None


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
    deviceAdded = QtCore.pyqtSignal(HIDDevice)
    deviceRemoved = QtCore.pyqtSignal(HIDDevice)

    logger = logging.getLogger('dsremap.HIDEnumerator')

    def __init__(self):
        super().__init__()

        self._devices = dict()
        self._stop = False

    def cancel(self):
        self._stop = True
        self.wait()

    def run(self):
        while not self._stop:
            found = set()
            for device in hid.enumerate():
                found.add(device['path'])
                if device['path'] not in self._devices and device['vendor_id'] == 0x54c:
                    dev = HIDDevice(device['path'], device['product_string'], device['vendor_id'], device['product_id'])
                    self.logger.info('Found device %04x/%04x', dev.vid, dev.pid)
                    handle = hid.device()
                    try:
                        try:
                            handle.open(dev.vid, dev.pid)
                            data = bytes(handle.get_feature_report(0x80, 7))
                            # Looks like on Windows we get "aligned" buffers (length 8 here)
                            _, major, minor, patch = struct.unpack('<BHHH', data[:7])
                            dev.fwVersion = Version(major, minor, patch)
                            self._devices[device['path']] = dev
                            self.deviceAdded.emit(dev)
                        finally:
                            handle.close()
                    except: # pylint: disable=W0702
                        self.logger.exception('Device %04x/%04x does not answer report 0x22', dev.vid, dev.pid)
            for path, dev in list(self._devices.items()):
                if path not in found:
                    self.logger.info('Device %04x/%04x unplugged', dev.vid, dev.pid)
                    del self._devices[path]
                    self.deviceRemoved.emit(dev)
            time.sleep(1)
