#!/usr/bin/env python3

import codecs
import os
import io
import struct

from pyqtcmd import History

from dsrlib.meta import Meta

from .listmodel import ListModel
from .persistence import JSONReader, JSONWriter


class Workspace:
    def __init__(self):
        super().__init__()
        self._history = History()
        self._configurations = ListModel()

    def history(self):
        return self._history

    def configurations(self):
        return self._configurations

    def bytecodeSize(self):
        total = 4 # At the end, 0-length configuration as sentinel, magic word at the start
        for configuration in self.configurations():
            if configuration.enabled():
                total += configuration.bytecodeSize()
        return total

    def bytecode(self):
        # The format of the data stored in the EEPROM is the following:
        #
        # uint16_t Magic word 0xCAFE
        # uin16_t little endian: length of the following configuration bytecode. 0 means the data ends here.
        #   uint16_t little endian: length of the following action
        #   action bytecode
        #   next action...
        # next configuration...

        bytecode = io.BytesIO()
        bytecode.write(struct.pack('<H', 0xCAFE))
        for configuration in self.configurations():
            if configuration.enabled():
                bytecode.write(configuration.bytecode())
        bytecode.write(struct.pack('<H', 0))
        return bytecode.getvalue()

    def load(self):
        filename = os.path.join(Meta.dataPath(), 'workspace.json')
        if os.path.exists(filename):
            reader = JSONReader()
            with codecs.getreader('utf-8')(open(filename, 'rb')) as fileobj:
                reader.read(fileobj, self)

    def save(self):
        filename = os.path.join(Meta.dataPath(), 'workspace.json')
        writer = JSONWriter()
        with codecs.getwriter('utf-8')(open(filename, 'wb')) as fileobj:
            writer.write(fileobj, self)

    def cleanup(self):
        files = set()
        for configuration in self.configurations():
            filename = configuration.thumbnail()
            if filename is None:
                continue
            files.add(os.path.basename(filename))

        for name in os.listdir(Meta.dataPath('thumbnails')):
            if name not in files:
                os.remove(os.path.join(Meta.dataPath('thumbnails'), name))
