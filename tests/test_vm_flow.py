#!/usr/bin/env python3

import unittest
import io
import struct

import base

from vmwrapper import VM, Report
from dsrlib.compiler.opcodes import Opcodes


class TestStack(unittest.TestCase):
    def setUp(self):
        self._bytecode = io.BytesIO()

    def add(self, data):
        self._bytecode.write(data)

    def add_opcode(self, subtype, variant=0):
        self.add(Opcodes.make_opcode(Opcodes.OPCODE_TYPE_FLOW, subtype, variant=variant))

    def create(self, stack=None, stacksize=None):
        if stacksize is None:
            stacksize = 0 if stack is None else len(stack)
        vm = VM(bytecode=self._bytecode.getvalue(), stacksize=stacksize)
        if stack is not None:
            vm.push(stack)
        return vm, self._bytecode.tell()

    def test_call(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_CALL)
        self.add(struct.pack('<H', 42))

        vm, size = self.create(stacksize=2)
        vm.step(Report())

        self.assertEqual(vm.offset, 42)
        self.assertEqual(vm.get_stack(), struct.pack('<H', 3))

    def test_ret(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_RET)

        vm, size = self.create(stack=struct.pack('<H', 42))
        ret = vm.step(Report())

        self.assertEqual(vm.offset, 42)
        self.assertEqual(vm.get_stack(), b'')
        self.assertFalse(ret)

    def test_yield(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_YIELD)

        vm, size = self.create()
        ret = vm.step(Report())

        self.assertEqual(vm.offset, size)
        self.assertTrue(ret)

    def test_jump(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_JUMP)
        self.add(struct.pack('<H', 42))

        vm, size = self.create()
        ret = vm.step(Report())

        self.assertEqual(vm.offset, 42)
        self.assertFalse(ret)

    def test_jz_C_int_true(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_JZ, variant=Opcodes.OPCODE_VARIANT_CI)
        self.add(struct.pack('<HH', 0, 42))

        vm, size = self.create()
        ret = vm.step(Report())

        self.assertEqual(vm.offset, 42)
        self.assertFalse(ret)

    def test_jz_C_int_false(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_JZ, variant=Opcodes.OPCODE_VARIANT_CI)
        self.add(struct.pack('<HH', 1, 42))

        vm, size = self.create()
        ret = vm.step(Report())

        self.assertEqual(vm.offset, size)
        self.assertFalse(ret)

    def test_jz_C_float_true(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_JZ, variant=Opcodes.OPCODE_VARIANT_CF)
        self.add(struct.pack('<fH', 0.0, 42))

        vm, size = self.create()
        ret = vm.step(Report())

        self.assertEqual(vm.offset, 42)
        self.assertFalse(ret)

    def test_jz_C_float_false(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_JZ, variant=Opcodes.OPCODE_VARIANT_CF)
        self.add(struct.pack('<fH', 1.0, 42))

        vm, size = self.create()
        ret = vm.step(Report())

        self.assertEqual(vm.offset, size)
        self.assertFalse(ret)

    def test_jz_reg_true(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_JZ, variant=Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_TH))
        self.add(struct.pack('<H', 42))

        vm, size = self.create()
        vm.TH = 0
        ret = vm.step(Report())

        self.assertEqual(vm.offset, 42)
        self.assertFalse(ret)

    def test_jz_reg_false(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_JZ, variant=Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_TH))
        self.add(struct.pack('<H', 42))

        vm, size = self.create()
        vm.TH = 1
        ret = vm.step(Report())

        self.assertEqual(vm.offset, size)
        self.assertFalse(ret)

    def test_jz_report_true(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_JZ, variant=Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_L2VALUE))
        self.add(struct.pack('<H', 42))

        vm, size = self.create()
        ret = vm.step(Report(L2Value=0))

        self.assertEqual(vm.offset, 42)
        self.assertFalse(ret)

    def test_jz_report_false(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_JZ, variant=Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_L2VALUE))
        self.add(struct.pack('<H', 42))

        vm, size = self.create()
        ret = vm.step(Report(L2Value=1))

        self.assertEqual(vm.offset, size)
        self.assertFalse(ret)

    def test_jz_constaddr_int_true(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_JZ, variant=Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 2, Opcodes.ADDR_VALTYPE_INT))
        self.add(struct.pack('<H', 42))

        vm, size = self.create(stack=struct.pack('<HHH', 1, 0, 1))
        ret = vm.step(Report())

        self.assertEqual(vm.offset, 42)
        self.assertFalse(ret)

    def test_jz_constaddr_int_false(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_JZ, variant=Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 2, Opcodes.ADDR_VALTYPE_INT))
        self.add(struct.pack('<H', 42))

        vm, size = self.create(stack=struct.pack('<HHH', 1, 1, 1))
        ret = vm.step(Report())

        self.assertEqual(vm.offset, size)
        self.assertFalse(ret)

    def test_jz_constaddr_float_true(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_JZ, variant=Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 2, Opcodes.ADDR_VALTYPE_FLOAT))
        self.add(struct.pack('<H', 42))

        vm, size = self.create(stack=struct.pack('<HfH', 1, 0.0, 1))
        ret = vm.step(Report())

        self.assertEqual(vm.offset, 42)
        self.assertFalse(ret)

    def test_jz_constaddr_float_false(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_JZ, variant=Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 2, Opcodes.ADDR_VALTYPE_FLOAT))
        self.add(struct.pack('<H', 42))

        vm, size = self.create(stack=struct.pack('<HfH', 1, 1.0, 1))
        ret = vm.step(Report())

        self.assertEqual(vm.offset, size)
        self.assertFalse(ret)

    def test_jz_regoff_int_true(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_JZ, variant=Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -4, Opcodes.ADDR_VALTYPE_INT))
        self.add(struct.pack('<H', 42))

        vm, size = self.create(stack=struct.pack('<HHH', 1, 0, 1))
        ret = vm.step(Report())

        self.assertEqual(vm.offset, 42)
        self.assertFalse(ret)

    def test_jz_regoff_int_false(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_JZ, variant=Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -4, Opcodes.ADDR_VALTYPE_INT))
        self.add(struct.pack('<H', 42))

        vm, size = self.create(stack=struct.pack('<HHH', 1, 1, 1))
        ret = vm.step(Report())

        self.assertEqual(vm.offset, size)
        self.assertFalse(ret)

    def test_jz_regoff_float_true(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_JZ, variant=Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -6, Opcodes.ADDR_VALTYPE_FLOAT))
        self.add(struct.pack('<H', 42))

        vm, size = self.create(stack=struct.pack('<HfH', 1, 0.0, 1))
        ret = vm.step(Report())

        self.assertEqual(vm.offset, 42)
        self.assertFalse(ret)

    def test_jz_regoff_float_false(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_FLOW_JZ, variant=Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -6, Opcodes.ADDR_VALTYPE_FLOAT))
        self.add(struct.pack('<H', 42))

        vm, size = self.create(stack=struct.pack('<HfH', 1, 1.0, 1))
        ret = vm.step(Report())

        self.assertEqual(vm.offset, size)
        self.assertFalse(ret)


if __name__ == '__main__':
    unittest.main()
