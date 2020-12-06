#!/usr/bin/env python3

import enum

@enum.unique
class PageId(enum.IntEnum):
    DeviceType = enum.auto()
    DeviceNotConfigured = enum.auto()
    ArduinoAvrdude = enum.auto()
    ArduinoReset = enum.auto()
    ArduinoFindSerial = enum.auto()
    PiZeroFind = enum.auto()
    PiZeroPlug = enum.auto()
    PiZeroPairHost = enum.auto()
    PiZeroWaitDS = enum.auto()
    PiZeroPairDS = enum.auto()
    PiZeroFinal = enum.auto()
