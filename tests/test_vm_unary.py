#!/usr/bin/env python3


import unittest
import io
import struct

import base

from vmwrapper import VM, Report
from dsrlib.compiler.opcodes import Opcodes


class TestUnary(unittest.TestCase):
    def setUp(self):
        self._bytecode = io.BytesIO()

    def add(self, data):
        self._bytecode.write(data)

    def add_opcode(self, subtype):
        self.add(Opcodes.make_opcode(Opcodes.OPCODE_TYPE_UNARY, subtype))

    def create(self, stack=None):
        vm = VM(bytecode=self._bytecode.getvalue(), stacksize=0 if stack is None else len(stack))
        if stack is not None:
            vm.push(stack)
        return vm, self._bytecode.tell()

    # NEG

    def test_neg_reg(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_UNARY_NEG)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_TH))

        vm, size = self.create()
        vm.TH = 42
        vm.step(Report())

        self.assertEqual(vm.TH, -42)
        self.assertEqual(vm.offset, size)

    def test_neg_constaddr_int(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_UNARY_NEG)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 2, Opcodes.ADDR_VALTYPE_INT))

        vm, size = self.create(stack=struct.pack('<hhh', 1, 2, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<hhh', 1, -2, 3))
        self.assertEqual(vm.offset, size)

    def test_neg_constaddr_float(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_UNARY_NEG)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 2, Opcodes.ADDR_VALTYPE_FLOAT))

        vm, size = self.create(stack=struct.pack('<hfh', 1, 2.5, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<hfh', 1, -2.5, 3))
        self.assertEqual(vm.offset, size)

    def test_neg_regoff_int(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_UNARY_NEG)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -4, Opcodes.ADDR_VALTYPE_INT))

        vm, size = self.create(stack=struct.pack('<hhh', 1, 2, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<hhh', 1, -2, 3))
        self.assertEqual(vm.offset, size)

    def test_neg_regoff_float(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_UNARY_NEG)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -6, Opcodes.ADDR_VALTYPE_FLOAT))

        vm, size = self.create(stack=struct.pack('<hfh', 1, 2.5, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<hfh', 1, -2.5, 3))
        self.assertEqual(vm.offset, size)

    # NOT

    def test_not_reg_true(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_UNARY_NOT)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_TH))

        vm, size = self.create()
        vm.TH = 42
        vm.step(Report())

        self.assertEqual(vm.TH, 0)
        self.assertEqual(vm.offset, size)

    def test_not_reg_false(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_UNARY_NOT)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_TH))

        vm, size = self.create()
        vm.TH = 0
        vm.step(Report())

        self.assertEqual(vm.TH, 1)
        self.assertEqual(vm.offset, size)

    def test_not_constaddr_int_true(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_UNARY_NOT)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 2, Opcodes.ADDR_VALTYPE_INT))

        vm, size = self.create(stack=struct.pack('<hhh', 1, 2, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<hhh', 1, 0, 3))
        self.assertEqual(vm.offset, size)

    def test_not_constaddr_int_false(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_UNARY_NOT)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 2, Opcodes.ADDR_VALTYPE_INT))

        vm, size = self.create(stack=struct.pack('<hhh', 1, 0, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<hhh', 1, 1, 3))
        self.assertEqual(vm.offset, size)

    def test_not_constaddr_float_true(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_UNARY_NOT)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 2, Opcodes.ADDR_VALTYPE_FLOAT))

        vm, size = self.create(stack=struct.pack('<hfh', 1, 2.5, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<hfh', 1, 0.0, 3))
        self.assertEqual(vm.offset, size)

    def test_not_constaddr_float_false(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_UNARY_NOT)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 2, Opcodes.ADDR_VALTYPE_FLOAT))

        vm, size = self.create(stack=struct.pack('<hfh', 1, 0.0, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<hfh', 1, 1.0, 3))
        self.assertEqual(vm.offset, size)

    def test_not_regoff_int_true(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_UNARY_NOT)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -4, Opcodes.ADDR_VALTYPE_INT))

        vm, size = self.create(stack=struct.pack('<hhh', 1, 2, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<hhh', 1, 0, 3))
        self.assertEqual(vm.offset, size)

    def test_not_regoff_int_false(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_UNARY_NOT)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -4, Opcodes.ADDR_VALTYPE_INT))

        vm, size = self.create(stack=struct.pack('<hhh', 1, 0, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<hhh', 1, 1, 3))
        self.assertEqual(vm.offset, size)

    def test_not_regoff_float_true(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_UNARY_NOT)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -6, Opcodes.ADDR_VALTYPE_FLOAT))

        vm, size = self.create(stack=struct.pack('<hfh', 1, 2.5, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<hfh', 1, 0.0, 3))
        self.assertEqual(vm.offset, size)

    def test_not_regoff_float_false(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_UNARY_NOT)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -6, Opcodes.ADDR_VALTYPE_FLOAT))

        vm, size = self.create(stack=struct.pack('<hfh', 1, 0.0, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<hfh', 1, 1.0, 3))
        self.assertEqual(vm.offset, size)


if __name__ == '__main__':
    unittest.main()
