#!/usr/bin/env python3

import unittest
import struct

import base

from dsrlib.compiler.opcodes import Opcodes


class TestRegAddr(unittest.TestCase):
    def test_reg_addr(self):
        value, = struct.unpack('>B', Opcodes.make_addr_reg(Opcodes.REGINDEX_TH))
        self.assertEqual(value, (Opcodes.ADDR_TYPE_REG << 6) | Opcodes.REGINDEX_TH)

    def test_reg_addr_str(self):
        value = Opcodes.make_addr_reg(Opcodes.REGINDEX_TH)
        self.assertEqual(Opcodes.make_addr_str(value).str, '%TH')


class TestRegAddrOffset(unittest.TestCase):
    def test_positive_int(self):
        value, = struct.unpack('>H', Opcodes.make_addr_regoff(Opcodes.REGINDEX_TH, 42, Opcodes.ADDR_VALTYPE_INT))
        self.assertEqual(value, (Opcodes.ADDR_TYPE_REGOFF << 14) | (Opcodes.REGINDEX_TH << 12) | (Opcodes.ADDR_VALTYPE_INT << 11) | 42)

    def test_negative_int(self):
        value, = struct.unpack('>H', Opcodes.make_addr_regoff(Opcodes.REGINDEX_TH, -42, Opcodes.ADDR_VALTYPE_INT))
        self.assertEqual(value, (Opcodes.ADDR_TYPE_REGOFF << 14) | (Opcodes.REGINDEX_TH << 12)| (Opcodes.ADDR_VALTYPE_INT << 11) | (1 << 10) | 42)

    def test_positive_float(self):
        value, = struct.unpack('>H', Opcodes.make_addr_regoff(Opcodes.REGINDEX_TH, 42, Opcodes.ADDR_VALTYPE_FLOAT))
        self.assertEqual(value, (Opcodes.ADDR_TYPE_REGOFF << 14) | (Opcodes.REGINDEX_TH << 12) | (Opcodes.ADDR_VALTYPE_FLOAT << 11) | 42)

    def test_negative_float(self):
        value, = struct.unpack('>H', Opcodes.make_addr_regoff(Opcodes.REGINDEX_TH, -42, Opcodes.ADDR_VALTYPE_FLOAT))
        self.assertEqual(value, (Opcodes.ADDR_TYPE_REGOFF << 14) | (Opcodes.REGINDEX_TH << 12)| (Opcodes.ADDR_VALTYPE_FLOAT << 11) | (1 << 10) | 42)

    def test_positive_int_str(self):
        value = Opcodes.make_addr_regoff(Opcodes.REGINDEX_TH, 42, Opcodes.ADDR_VALTYPE_INT)
        self.assertEqual(Opcodes.make_addr_str(value).str, '[%TH+42]i')

    def test_negative_int_str(self):
        value = Opcodes.make_addr_regoff(Opcodes.REGINDEX_TH, -42, Opcodes.ADDR_VALTYPE_INT)
        self.assertEqual(Opcodes.make_addr_str(value).str, '[%TH-42]i')

    def test_positive_float_str(self):
        value = Opcodes.make_addr_regoff(Opcodes.REGINDEX_TH, 42, Opcodes.ADDR_VALTYPE_FLOAT)
        self.assertEqual(Opcodes.make_addr_str(value).str, '[%TH+42]f')

    def test_negative_float_str(self):
        value = Opcodes.make_addr_regoff(Opcodes.REGINDEX_TH, -42, Opcodes.ADDR_VALTYPE_FLOAT)
        self.assertEqual(Opcodes.make_addr_str(value).str, '[%TH-42]f')


if __name__ == '__main__':
    unittest.main()
