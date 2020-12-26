#!/usr/bin/env python3

import enum

@enum.unique
class PageId(enum.IntEnum):
    DeviceType = enum.auto()
    DeviceNotConfigured = enum.auto()
    ArduinoAvrdude = enum.auto()
    ArduinoManifestDownload = enum.auto()
    ArduinoDownload = enum.auto()
    ArduinoReset = enum.auto()
    ArduinoFindSerial = enum.auto()
    PiZeroManifestDownload = enum.auto()
    PiZeroImageDownload = enum.auto()
    PiZeroBurn = enum.auto()
    PiZeroWifi = enum.auto()
    PiZeroCopy = enum.auto()
    PiZeroFind = enum.auto()
    PiZeroPlug = enum.auto()
    PiZeroPairHost = enum.auto()
    PiZeroWaitDS = enum.auto()
    PiZeroPairDS = enum.auto()
    PiZeroFinal = enum.auto()
