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

    def add_opcode(self, subtype):
        self.add(Opcodes.make_opcode(Opcodes.OPCODE_TYPE_STACK, subtype))

    def create(self, stack=None):
        vm = VM(bytecode=self._bytecode.getvalue(), stacksize=0 if stack is None else len(stack))
        if stack is not None:
            vm.push(stack)
        return vm, self._bytecode.tell()

    def test_pushi(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_STACK_PUSHI)
        self.add(struct.pack('<i', 42))

        vm, size = self.create(stack=struct.pack('<i', 1))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<ii', 1, 42))
        self.assertEqual(vm.offset, size)

    def test_pushf(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_STACK_PUSHF)
        self.add(struct.pack('<f', 42.5))

        vm, size = self.create(stack=struct.pack('<i', 1))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<if', 1, 42.5))
        self.assertEqual(vm.offset, size)

    def test_push_reg(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_STACK_PUSH)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_TH))

        vm, size = self.create(stack=struct.pack('<i', 1))
        vm.TH = 42
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<ii', 1, 42))
        self.assertEqual(vm.offset, size)

    def test_push_report_int(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_STACK_PUSH)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_L2VALUE))

        vm, size = self.create(stack=struct.pack('<i', 1))
        vm.step(Report(L2Value=13))

        self.assertEqual(vm.get_stack(), struct.pack('<ii', 1, 13))
        self.assertEqual(vm.offset, size)

    def test_push_report_float(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_STACK_PUSH)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_IMUY))

        vm, size = self.create(stack=struct.pack('<i', 1))
        vm.step(Report(IMUY=13.5))

        self.assertEqual(vm.get_stack(), struct.pack('<if', 1, 13.5))
        self.assertEqual(vm.offset, size)

    def test_push_report_constaddr_int(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_STACK_PUSH)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 4, Opcodes.ADDR_VALTYPE_INT))

        vm, size = self.create(stack=struct.pack('<iii', 1, 42, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<iiii', 1, 42, 3, 42))
        self.assertEqual(vm.offset, size)

    def test_push_report_constaddr_float(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_STACK_PUSH)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 4, Opcodes.ADDR_VALTYPE_FLOAT))

        vm, size = self.create(stack=struct.pack('<ifi', 1, 42.5, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<ifif', 1, 42.5, 3, 42.5))
        self.assertEqual(vm.offset, size)

    def test_push_report_regoff_int(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_STACK_PUSH)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -8, Opcodes.ADDR_VALTYPE_INT))

        vm, size = self.create(stack=struct.pack('<iii', 1, 42, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<iiii', 1, 42, 3, 42))
        self.assertEqual(vm.offset, size)

    def test_push_report_regoff_float(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_STACK_PUSH)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -8, Opcodes.ADDR_VALTYPE_FLOAT))

        vm, size = self.create(stack=struct.pack('<ifi', 1, 42.5, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<ifif', 1, 42.5, 3, 42.5))
        self.assertEqual(vm.offset, size)

    def test_pop_reg(self):
        self.add_opcode(Opcodes.OPCODE_SUBTYPE_STACK_POP)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_TH))

        vm, size = self.create(stack=struct.pack('<ii', 1, 42))
        vm.TH = 5
        vm.step(Report())

        self.assertEqual(vm.TH, 42)
        self.assertEqual(vm.get_stack(), struct.pack('<i', 1))
        self.assertEqual(vm.offset, size)

    # This is the only kind of POP right now.


if __name__ == '__main__':
    unittest.main()
