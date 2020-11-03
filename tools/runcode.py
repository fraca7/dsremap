#!/usr/bin/env python3

import os
import sys
import codecs
import struct

sys.path.insert(0, os.path.join('..', 'src'))
sys.path.insert(0, os.path.join('..', 'src', 'ext'))

from dsrlib.compiler.bcgen import generateBytecode
from vmwrapper import VM, Report


def main(argv):
    filename, = argv
    with codecs.getreader('utf-8')(open(filename, 'rb')) as fileobj:
        source = fileobj.read()
    _, bytecode = generateBytecode(source)
    bytecode = bytecode.getvalue()

    stacksize, = struct.unpack('<H', bytecode[:2])
    vm = VM(bytecode=bytecode[2:], stacksize=stacksize)

    print('!! Stack size:', stacksize)

    for index in range(100):
        report = Report()
        report.L2 = report.R2 = True

        print('!! RUN')
        while not vm.step(report):
            pass


if __name__ == '__main__':
    main(sys.argv[1:])
