#!/usr/bin/env python3

import os
import time
import codecs
import getopt
import sys
import pickle


def write_ids(defname, dstname, strname):
    current = 0
    strings = {}
    seen = set()
    with codecs.getreader('utf-8')(open(defname, 'rb')) as src:
        with codecs.getwriter('utf-8')(open(dstname, 'wb')) as dst:
            name = os.path.basename(dstname).upper().replace('.', '_')
            dst.write('\n#ifndef %s\n' % name)
            dst.write('#define %s\n' % name)
            dst.write('\n/* Generated on %s */\n\n' % time.ctime())

            for line in src:
                line = line.strip()
                if not line:
                    dst.write('\n')
                    continue
                if line.startswith('#'):
                    dst.write('/* %s */\n' % line[1:].strip())
                    continue

                idx = line.index(' ')
                name, value = line[:idx], line[idx:].strip()
                if value in seen:
                    raise ValueError('Duplicate string ID "%s"' % value)
                seen.add(value)

                dst.write('#define %s %d // %s\n' % (name, current, value))
                strings[current] = value
                current += 1
                if current == 65536:
                    raise ValueError('Too much string IDs')

            dst.write('#endif\n')

    with open(strname, 'wb') as fileobj:
        pickle.dump(strings, fileobj)


def usage(code=1):
    print('Usage: %s -o output_filename -s strings_filename input_filename' % sys.argv[0])
    sys.exit(code)


def main(argv):
    try:
        opts, args = getopt.getopt(argv, 'ho:s:', ['help', 'output=', 'strings='])
    except getopt.Error as exc:
        print(exc)
        usage()

    if len(args) == 0:
        print('No input file specified')
        usage()
    if len(args) > 1:
        print('Too many arguments')
        usage()

    outname = None
    strname = None
    for opt, val in opts:
        if opt in ('-h', '--help'):
            usage(0)
        if opt in ('-o', '--output'):
            outname = val
        if opt in ('-s', '--strings'):
            strname = val

    if outname is None:
        print('No output file specified')
        usage()
    if strname is None:
        print('No strings file specified')
        usage()

    try:
        write_ids(args[0], outname, strname)
    except Exception as exc:
        print(exc)
        usage()


if __name__ == '__main__':
    main(sys.argv[1:])
