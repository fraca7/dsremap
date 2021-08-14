#!/usr/bin/env python3

import sys
import argparse
import contextlib
import collections
import random
import binascii
import struct

import hid

"""
Dualshock command-line utility
"""


PairingInfo = collections.namedtuple('PairingInfo', ['addr', 'paired_to'])
IMUCalib = collections.namedtuple('IMUCalib', ['gxbias', 'gybias', 'gzbias', 'gxp', 'gxm', 'gyp', 'gym', 'gzp', 'gzm', 'gsp', 'gsm', 'axp', 'axm', 'ayp', 'aym', 'azp', 'azm'])

class BTAddr:
    def __init__(self, binary):
        self.binary = binary
        self.string = ':'.join(['%02X' % val for val in reversed(binary)])

    @classmethod
    def from_string(cls, value):
        return BTAddr(bytes(reversed(binascii.unhexlify(value.replace(':', '')))))


class Dualshock:
    def __init__(self, pid, handle):
        self.pid = pid
        self.handle = handle

    def pair(self, remote_addr, key):
        self.handle.send_feature_report(b'\x13' + remote_addr.binary + key)

    def get_pairing_info(self):
        data = bytes(self.handle.get_feature_report(0x12, 64))
        return PairingInfo(BTAddr(data[1:7]), BTAddr(data[10:16]))

    def get_btaddr(self):
        data = bytes(self.handle.get_feature_report(0x81, 64))
        return BTAddr(data[1:])

    def get_imu_calibration(self):
        data = bytes(self.handle.get_feature_report(0x02, 64))
        return IMUCalib(*struct.unpack('<%s' % ('h' * 17), data[1:-2]))


@contextlib.contextmanager
def find_dualshock():
    for dev in hid.enumerate():
        vid, pid = dev['vendor_id'], dev['product_id']
        if vid == 0x054c and pid == 0x09cc:
            handle = hid.device()
            handle.open(vid, pid)
            try:
                yield Dualshock(pid, handle)
            finally:
                handle.close()
            break
    else:
        raise RuntimeError('No Dualshock found')


def show_info(args):
    with find_dualshock() as ds4:
        print('Controller (PID=0x%04x)' % ds4.pid)

        info = ds4.get_pairing_info()
        imu = ds4.get_imu_calibration()
        print('  BT address:          %s' % info.addr.string)
        print('  Currently paired to: %s' % info.paired_to.string)
        print('  IMU calibration:')
        print('    Gyro')
        print('      Bias: %d - %d - %d' % (imu.gxbias, imu.gybias, imu.gzbias))
        print('      Max: %d - %d - %d' % (imu.gxp, imu.gyp, imu.gzp))
        print('      Min: %d - %d - %d' % (imu.gxm, imu.gym, imu.gzm))
        print('      Speed: %d - %d' % (imu.gsp, imu.gsm))
        print('    Accelerometer')
        print('      Max: %d - %d - %d' % (imu.axp, imu.ayp, imu.azp))
        print('      Min: %d - %d - %d' % (imu.axm, imu.aym, imu.azm))


def pair_controller(args):
    if args.key is None:
        key = bytes([random.randint(0, 255) for idx in range(16)])
    else:
        key = binascii.unhexlify(args.key)
        if len(key) != 16:
            raise RuntimeError('Invalid key length')

    with find_dualshock() as ds4:
        ds4.pair(BTAddr.from_string(args.btaddr), key)
        if args.key is None:
            print('Link key: %s' % binascii.hexlify(key).decode('ascii').upper())


def main(argv):
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers()

    parser_info = subparsers.add_parser('info', help='Display controller information')
    parser_info.set_defaults(func=show_info)

    parser_pair = subparsers.add_parser('pair', help='Set controller pairing state')
    parser_pair.add_argument('btaddr', help='BT address of the host')
    parser_pair.add_argument('-k', '--key', help='Link key (16 bytes, hex)', default=None)
    parser_pair.set_defaults(func=pair_controller)

    args = parser.parse_args(argv)
    args.func(args)


if __name__ == '__main__':
    main(sys.argv[1:])
