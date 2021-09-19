#!/usr/bin/env python3

import sys
import argparse
import contextlib
import collections
import binascii
import struct
import json

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


class Controller:
    def __init__(self, pid, handle):
        self.pid = pid
        self.handle = handle

    def name(self):
        raise NotImplementedError

    def pair(self, remote_addr, key):
        raise NotImplementedError

    def get_pairing_info(self):
        raise NotImplementedError

    def get_imu_calibration(self):
        raise NotImplementedError

    def known_reports(self):
        raise NotImplementedError

    def show_info(self):
        print(self.name())

        info = self.get_pairing_info()
        imu = self.get_imu_calibration()
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

    def print_report(self, report_id):
        data = bytes(self.handle.get_feature_report(report_id, 64))
        print('Report 0x%02x; length=%d' % (report_id, len(data)))
        for offset in range((len(data) + 15) // 16):
            s1 = ' '.join(['%02x' % value for value in data[offset * 16:offset * 16 + 16]])
            s2 = ''.join([chr(value) if 32 <= value < 64 else '.' for value in data[offset * 16:offset * 16 + 16]])
            print('%04x %-47s %s' % (offset * 16, s1, s2))

    def extract_reports(self, addr=None):
        data = { 'pid': self.pid, 'reports': {} }
        for report_id in self.known_reports():
            report = list(self.handle.get_feature_report(report_id, 64))
            if addr is not None:
                self._replace_addr(report, addr)
            data['reports']['%02x' % report_id] = binascii.hexlify(bytes(report)).decode('ascii')
        print(json.dumps(data, indent=2))

    def _replace_addr(self, report, addr):
        raise NotImplementedError


class Dualshock(Controller):
    def name(self):
        return 'Dualshock (PID=0x%04x)' % self.pid

    def pair(self, remote_addr, key):
        self.handle.send_feature_report(b'\x13' + remote_addr.binary + key)

    def get_pairing_info(self):
        data = bytes(self.handle.get_feature_report(0x12, 64))
        return PairingInfo(BTAddr(data[1:7]), BTAddr(data[10:16]))

    def get_imu_calibration(self):
        data = bytes(self.handle.get_feature_report(0x02, 64))
        return IMUCalib(*struct.unpack('<%s' % ('h' * 17), data[1:-2]))

    def known_reports(self):
        return (0x02, 0xa3, 0x12, 0x81)

    def _replace_addr(self, report, addr):
        if report[0] == 0x12:
            report[1:7] = addr.binary


class Dualsense(Controller):
    def name(self):
        return 'Dualsense (PID=0x%04x)' % self.pid

    def pair(self, remote_addr, key):
        self.handle.send_feature_report(b'\x0a' + remote_addr.binary + key)

    def get_pairing_info(self):
        data = bytes(self.handle.get_feature_report(0x09, 64))
        return PairingInfo(BTAddr(data[1:7]), BTAddr(data[10:16]))

    def get_imu_calibration(self):
        data = bytes(self.handle.get_feature_report(0x05, 64))
        return IMUCalib(*struct.unpack('<%s' % ('h' * 17), data[1:35]))

    def known_reports(self):
        return (0x05, 0x20, 0x09)

    def _replace_addr(self, report, addr):
        if report[0] == 0x09:
            report[1:7] = addr.binary


@contextlib.contextmanager
def find_controller():
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
        elif vid == 0x054c and pid == 0x0ce6:
            handle = hid.device()
            handle.open(vid, pid)
            try:
                yield Dualsense(pid, handle)
            finally:
                handle.close()
            break
    else:
        raise RuntimeError('No Dualshock found')


def show_info(args):
    with find_controller() as ctrl:
        ctrl.show_info()


def pair_controller(args):
    if args.key is None:
        key = b'\xde\xad\xbe\xef\xde\xad\xbe\xef\xde\xad\xbe\xef\xde\xad\xbe\xef'
    else:
        key = binascii.unhexlify(args.key)
        if len(key) != 16:
            raise RuntimeError('Invalid key length')

    with find_controller() as ctrl:
        ctrl.pair(BTAddr.from_string(args.btaddr), key)
        if args.key is None:
            print('Link key: %s' % binascii.hexlify(key).decode('ascii').upper())


def print_report(args):
    report_id = int(args.reportid, 16) if args.reportid.startswith('0x') else int(args.reportid)
    with find_controller() as ctrl:
        ctrl.print_report(report_id)


def extract_reports(args):
    with find_controller() as ctrl:
        ctrl.extract_reports(addr=None if args.address is None else BTAddr.from_string(args.address))


def main(argv):
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers()

    parser_info = subparsers.add_parser('info', help='Display controller information')
    parser_info.set_defaults(func=show_info)

    parser_pair = subparsers.add_parser('pair', help='Set controller pairing state')
    parser_pair.add_argument('btaddr', help='BT address of the host')
    parser_pair.add_argument('-k', '--key', help='Link key (16 bytes, hex)', default=None)
    parser_pair.set_defaults(func=pair_controller)

    parser_print = subparsers.add_parser('print', help='Print a specific feature report')
    parser_print.add_argument('reportid', help='Report ID in either hex (with 0x prefix) or decimal')
    parser_print.set_defaults(func=print_report)

    parser_extract = subparsers.add_parser('extract', help='Extract all known reports in JSON format')
    parser_extract.add_argument('-a', '--address', help='Replace controller MAC address with this one in the pairing report', default=None)
    parser_extract.set_defaults(func=extract_reports)

    args = parser.parse_args(argv)
    args.func(args)


if __name__ == '__main__':
    main(sys.argv[1:])
