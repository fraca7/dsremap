#!/usr/bin/env python3

import os
import io
import time
import sys
import struct
import collections
import pickle
import getopt
import zlib

import colorama
import serial


class CRC32Checker:
    def __init__(self):
        self._value = 0

    def update(self, byte):
        self._value = zlib.crc32(bytes([byte]), self._value)

    def check(self, data):
        return self._value == struct.unpack('<I', data)[0]

    def size(self):
        return 4


# Sans IO parser for the binary log format
class LogParser:
    Header = collections.namedtuple('Header', ['level', 'params', 'msgid'])

    typeMap = {
        0x01: 'b',
        0x02: 'B',
        0x03: 'h',
        0x04: 'H',
        0x05: 'i',
        0x06: 'I',
        0x07: 'f',
        }

    def __init__(self, listener):
        self.listener = listener
        self.state = 0
        self.buffer = None
        self.data = None
        self.header = None
        self.types = []
        self.params = []
        self.ck = None

    def feed(self, byte):
        if self.state == 0:
            if byte == 0xFE:
                self.state = 1
        elif self.state == 1:
            if byte == 0xCA:
                self.buffer = io.BytesIO()
                self.header = None
                self.ck = CRC32Checker()
                self.state = 2
            elif byte != 0xFE:
                self.state = 0
        elif self.state == 2: # Receiving header
            self.ck.update(byte)
            self.buffer.write(bytes([byte]))
            if self.buffer.tell() == 4:
                self.header = self.Header(*struct.unpack('<BBH', self.buffer.getvalue()))
                if self.header.msgid in (0xFFFF, 0xFFFE, 0xFFFD):
                    # Special case: raw buffer
                    self.buffer = io.BytesIO()
                    self.state = 6
                elif self.header.params:
                    self.buffer = io.BytesIO()
                    self.state = 3
                else:
                    self.params = []
                    self.buffer = io.BytesIO()
                    self.state = 5
        elif self.state == 3: # Receiving parameter types
            self.buffer.write(bytes([byte]))
            if self.buffer.tell() * 2 >= self.header.params:
                self.types = []
                for byte in self.buffer.getvalue():
                    self.ck.update(byte)
                    for shift in (0, 4):
                        val = (byte >> shift) & 15
                        if val:
                            try: # WTF ?
                                self.types.append((val, struct.calcsize(self.typeMap[val])))
                            except KeyError:
                                print('!! TYPE ERROR %d' % val)
                                self.state = 0
                                return
                self.params = []
                self.buffer = io.BytesIO()
                self.state = 4
        elif self.state == 4: # Reading params
            self.ck.update(byte)
            self.buffer.write(bytes([byte]))
            val, count = self.types[0]
            if self.buffer.tell() == count:
                self.params.append(struct.unpack('<%s' % self.typeMap[val], self.buffer.getvalue())[0])
                self.types.pop(0)
                self.buffer = io.BytesIO()
                if not self.types:
                    self.state = 5
        elif self.state == 5:
            self.buffer.write(bytes([byte]))
            if self.buffer.tell() == self.ck.size():
                if self.ck.check(self.buffer.getvalue()):
                    # Got it
                    self.listener.on_message(self.header.level, self.header.msgid, tuple(self.params))
                else:
                    print('!! Checksum failed')
                self.state = 0
        elif self.state == 6:
            self.ck.update(byte)
            self.buffer.write(bytes([byte]))
            if self.buffer.tell() == self.header.params:
                self.data = self.buffer.getvalue()
                self.buffer = io.BytesIO()
                self.state = 7
        elif self.state == 7:
            self.buffer.write(bytes([byte]))
            if self.buffer.tell() == self.ck.size():
                if self.ck.check(self.buffer.getvalue()):
                    self.listener.on_buffer(self.header.msgid, self.data)
                else:
                    print('!! Checksum failed')
                self.state = 0


class SerialLogParser(LogParser):
    def __init__(self, listener, path):
        super().__init__(listener)
        self._path = path
        self._stopping = False

    def stop(self):
        self._stopping = True

    def run(self):
        while not self._stopping:
            while not self._stopping:
                try:
                    ser = serial.Serial(self._path, 115200, timeout=1)
                    break
                except Exception as exc:
                    time.sleep(0.1)

            self.listener.on_connected()

            try:
                while not self._stopping:
                    for byte in ser.read(64):
                        self.feed(byte)
            except Exception as exc:
                self.listener.on_error(exc)


class SerialLogPrinter(SerialLogParser):
    def __init__(self, path):
        super().__init__(self, path)

        if not os.path.exists('strings.pickle'):
            raise RuntimeError('No strings.pickle; run make in the toplevel directory first')
        with open('strings.pickle', 'rb') as fileobj:
            self._strings = pickle.load(fileobj)

    def on_connected(self):
        print('%s!! Connected.' % colorama.Fore.WHITE)

    def on_error(self, exc):
        print('%s!! ERROR: %s' % (colorama.Fore.RED, exc))

    def on_message(self, level, msgid, params):
        fmt = self._strings[msgid]
        level_str = {1: 'E', 2: 'W', 3: 'I', 4: 'D', 5: 'T'}[level]
        color = {1: colorama.Fore.RED, 2: colorama.Fore.YELLOW, 3: colorama.Fore.WHITE, 4: colorama.Fore.GREEN, 5: colorama.Fore.BLUE}[level]
        try:
            print('%s%s: %s' % (color, level_str, fmt % params))
        except Exception as exc:
            print('%sERROR: %s when formatting "%s" with %s' % (colorama.Fore.RED, exc, fmt, str(params)))

    def on_buffer(self, msgid, data):
        if msgid != 0xFFFF:
            return

        while data:
            count = min(len(data), 16)
            print('B: %s' % ' '.join(['%02x' % byte for byte in data[:count]]))
            data = data[count:]


def usage(code=1):
    print('Usage: %s [options] path_to_serial' % sys.argv[0])
    print('Options:')
    print('  -h, --help         Display this')
    sys.exit(code)


def main(argv):
    try:
        opts, args = getopt.getopt(argv, 'h', ['help'])
    except getopt.error as exc:
        print(exc)
        usage()

    if len(args) == 0:
        print('No serial path specified')
        usage()
    if len(args) > 1:
        print('Too many arguments')
        usage()

    for opt, val in opts:
        if opt in ('-h', '--help'):
            usage(0)

    path, = args
    colorama.init()
    p = SerialLogPrinter(path)
    try:
        p.run()
    except KeyboardInterrupt:
        pass


if __name__ == '__main__':
    main(sys.argv[1:])
