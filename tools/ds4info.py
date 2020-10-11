#!/usr/bin/env python3

import binascii

import hid


for dev in hid.enumerate():
    vid, pid = dev['vendor_id'], dev['product_id']
    if vid == 0x054c and pid == 0x09cc:
        handle = hid.device()
        handle.open(vid, pid)
        try:
            data = bytes(handle.get_feature_report(0x12, 64))
            btaddr = ':'.join(['%02x' % val for val in reversed(data[1:7])])
            paired = ':'.join(['%02x' % val for val in reversed(data[10:16])])
            print('BT addr: %s' % btaddr.upper())
            print('Currently paired to %s' % paired.upper())
            data = bytes(handle.get_feature_report(0x02, 64))
            print('IMU: %s' % ' '.join(['%02x' % val for val in data[1:]]))
        finally:
            handle.close()
        break
else:
    print('No DualShock found')
