#!/usr/bin/env python3

import sys
import binascii

import hid

if len(sys.argv) != 3:
    print('Usage: pairds4.py <BT addr> <linkkey>')
    sys.exit(1)

btaddr = bytes(reversed(binascii.unhexlify(sys.argv[1].replace(':', ''))))
linkkey = binascii.unhexlify(sys.argv[2])

for dev in hid.enumerate():
    if dev['vendor_id'] == 0x054c and dev['product_id'] == 0x09cc:
        print('Found DS4')
        handle = hid.device()
        handle.open(dev['vendor_id'], dev['product_id'])
        try:
            data = handle.get_feature_report(0x81, 64)
            print('BT addr: %s' % ':'.join(['%02X' % val for val in reversed(data[1:])]))
            print('Pairing')
            handle.send_feature_report(b'\x13' + btaddr + linkkey)
            print('Done.')
        finally:
            handle.close()
        break
else:
    print('No DualShock found')
