#!/usr/bin/env python3

import unittest
import io
import struct

import base

from vmwrapper import VM, Report
from dsrlib.compiler.opcodes import Opcodes


class TestCast(unittest.TestCase):
    def setUp(self):
        self._bytecode = io.BytesIO()

    def add(self, data):
        self._bytecode.write(data)

    def add_opcode(self):
        self.add(Opcodes.make_opcode(Opcodes.OPCODE_TYPE_BINARY, Opcodes.OPCODE_SUBTYPE_BINARY_CAST, Opcodes.OPCODE_VARIANT_A))

    def create(self, stack=None):
        vm = VM(bytecode=self._bytecode.getvalue(), stacksize=0 if stack is None else len(stack))
        if stack is not None:
            vm.push(stack)
        return vm, self._bytecode.tell()

    # In theory we don't need to support a const second operand, those are cast at compile time

    def test_report_int_report_float(self):
        self.add_opcode()
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_L2VALUE))
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_IMUX))

        vm, size = self.create()
        report = Report(IMUX=42.5)
        vm.step(report)

        self.assertEqual(report.L2Value, 42)
        self.assertEqual(vm.offset, size)

    def test_regoff_int_regoff_float(self):
        self.add_opcode()
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 4, Opcodes.ADDR_VALTYPE_INT))
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 8, Opcodes.ADDR_VALTYPE_FLOAT))

        vm, size = self.create(stack=struct.pack('<iif', 1, 13, 42.5))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<iif', 1, 42, 42.5))
        self.assertEqual(vm.offset, size)

    def test_regoff_float_regoff_int(self):
        self.add_opcode()
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 4, Opcodes.ADDR_VALTYPE_FLOAT))
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 8, Opcodes.ADDR_VALTYPE_INT))

        vm, size = self.create(stack=struct.pack('<ifi', 1, 42.5, 13))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<ifi', 1, 13.0, 13))
        self.assertEqual(vm.offset, size)

    def test_reg_report(self):
        self.add_opcode()
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_TH))
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_IMUX))

        vm, size = self.create()
        vm.step(Report(IMUX=42.5))

        self.assertEqual(vm.TH, 42)
        self.assertEqual(vm.offset, size)

    def test_reg_int_regoff_float(self):
        self.add_opcode();
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_TH))
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 4, Opcodes.ADDR_VALTYPE_FLOAT))

        vm, size = self.create(stack=struct.pack('<if', 1, 42.5))
        vm.step(Report())

        self.assertEqual(vm.TH, 42)
        self.assertEqual(vm.offset, size)


class TestAdd(unittest.TestCase):
    def setUp(self):
        self._bytecode = io.BytesIO()

    def add(self, data):
        self._bytecode.write(data)

    def add_opcode(self, variant):
        self.add(Opcodes.make_opcode(Opcodes.OPCODE_TYPE_BINARY, Opcodes.OPCODE_SUBTYPE_BINARY_ADD, variant))

    def create(self, stack=None):
        vm = VM(bytecode=self._bytecode.getvalue(), stacksize=0 if stack is None else len(stack))
        if stack is not None:
            vm.push(stack)
        return vm, self._bytecode.tell()

    # Constant source

    def test_reg_C(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_C)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_SP))
        self.add(struct.pack('<i', 42))

        vm, size = self.create(stack=b'\x00\x00')
        vm.step(Report())

        self.assertEqual(vm.SP, 44)
        self.assertEqual(vm.offset, size)

    def test_report_C(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_C)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_L2VALUE))
        self.add(struct.pack('<i', 42))

        vm, size = self.create()
        report = Report(L2Value=9)
        vm.step(report)

        self.assertEqual(report.L2Value, 9 + 42)
        self.assertEqual(vm.offset, size)

    def test_constaddr_C_int(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_C)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 9, Opcodes.ADDR_VALTYPE_INT))
        self.add(struct.pack('<i', 42))

        vm, size = self.create(stack=struct.pack('<iibii', 1, 2, 3, 4, 5))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<iibii', 1, 2, 3, 4 + 42, 5))
        self.assertEqual(vm.offset, size)

    def test_constaddr_C_float(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_C)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 9, Opcodes.ADDR_VALTYPE_FLOAT))
        self.add(struct.pack('<f', 42.5))

        vm, size = self.create(stack=struct.pack('<iibfi', 1, 2, 3, 4.0, 5))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<iibfi', 1, 2, 3, 4.0 + 42.5, 5))
        self.assertEqual(vm.offset, size)

    def test_regoff_C_int(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_C)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -8, Opcodes.ADDR_VALTYPE_INT))
        self.add(struct.pack('<i', 42))

        vm, size = self.create(stack=struct.pack('<iii', 1, 2, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<iii', 1, 2 + 42, 3))
        self.assertEqual(vm.offset, size)

    def test_regoff_C_float(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_C)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -8, Opcodes.ADDR_VALTYPE_FLOAT))
        self.add(struct.pack('<f', 42.5))

        vm, size = self.create(stack=struct.pack('<ifi', 1, 2.5, 3))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<ifi', 1, 2.5 + 42.5, 3))
        self.assertEqual(vm.offset, size)

    # Same source/target

    def test_reg_reg(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_SP))
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_TH))

        vm, size = self.create(stack=b'\x00\x00')
        vm.TH = 42
        vm.step(Report())

        self.assertEqual(vm.SP, 2 + 42)
        self.assertEqual(vm.offset, size)

    def test_report_report(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_L2VALUE))
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_R2VALUE))

        vm, size = self.create()
        report = Report(L2Value=9, R2Value=42)
        vm.step(report)

        self.assertEqual(report.L2Value, 9 + 42)
        self.assertEqual(vm.offset, size)

    def test_constaddr_constaddr_int(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 4, Opcodes.ADDR_VALTYPE_INT))
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 8, Opcodes.ADDR_VALTYPE_INT))

        vm, size = self.create(stack=struct.pack('<iii', 1, 13, 42))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<iii', 1, 13 + 42, 42))
        self.assertEqual(vm.offset, size)

    def test_constaddr_constaddr_float(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 4, Opcodes.ADDR_VALTYPE_FLOAT))
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 8, Opcodes.ADDR_VALTYPE_FLOAT))

        vm, size = self.create(stack=struct.pack('<iff', 1, 13.5, 42.5))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<iff', 1, 13.5 + 42.5, 42.5))
        self.assertEqual(vm.offset, size)

    def test_regoff_regoff_int(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -8, Opcodes.ADDR_VALTYPE_INT))
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -12, Opcodes.ADDR_VALTYPE_INT))

        vm, size = self.create(stack=struct.pack('<iiii', 1, 2, 3, 4))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<iiii', 1, 2, 5, 4))
        self.assertEqual(vm.offset, size)

    def test_regoff_regoff_float(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -8, Opcodes.ADDR_VALTYPE_FLOAT))
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -12, Opcodes.ADDR_VALTYPE_FLOAT))

        vm, size = self.create(stack=struct.pack('<iffi', 1, 2.5, 3.5, 4))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<iffi', 1, 2.5, 6.0, 4))
        self.assertEqual(vm.offset, size)

    # Reg target

    def test_reg_report(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_SP))
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_L2VALUE))

        vm, size = self.create(stack=b'\x00\x00')
        vm.step(Report(L2Value=42))

        self.assertEqual(vm.SP, 44)
        self.assertEqual(vm.offset, size)

    def test_reg_constaddr(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_SP))
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 9, Opcodes.ADDR_VALTYPE_INT))

        vm, size = self.create(stack=struct.pack('<iibii', 1, 2, 3, 42, 13))
        vm.step(Report())

        self.assertEqual(vm.SP, 17 + 42)
        self.assertEqual(vm.offset, size)

    def test_reg_regoff(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_SP))
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -8, Opcodes.ADDR_VALTYPE_INT))

        vm, size = self.create(stack=struct.pack('<iii', 1, 42, 3))
        vm.step(Report())

        self.assertEqual(vm.SP, 12 + 42)
        self.assertEqual(vm.offset, size)

    # Report target

    def test_report_reg(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_L2VALUE))
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_TH))

        vm, size = self.create()
        vm.TH = 42
        report = Report(L2Value=9)
        vm.step(report)

        self.assertEqual(report.L2Value, 9 + 42)
        self.assertEqual(vm.offset, size)

    def test_report_constaddr(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_L2VALUE))
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 9, Opcodes.ADDR_VALTYPE_INT))

        vm, size = self.create(stack=struct.pack('<iibii', 1, 2, 3, 42, 13))
        report = Report(L2Value=9)
        vm.step(report)

        self.assertEqual(report.L2Value, 9 + 42)
        self.assertEqual(vm.offset, size)

    def test_report_regoff(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_L2VALUE))
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -8, Opcodes.ADDR_VALTYPE_INT))

        vm, size = self.create(stack=struct.pack('<iii', 1, 42, 3))
        report = Report(L2Value=9)
        vm.step(report)

        self.assertEqual(report.L2Value, 9 + 42)
        self.assertEqual(vm.offset, size)

    # constaddr target

    def test_constaddr_reg(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 4, Opcodes.ADDR_VALTYPE_INT))
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_TH))

        vm, size = self.create(stack=struct.pack('<ii', 1, 2))
        vm.TH = 42;
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<ii', 1, 2 + 42))
        self.assertEqual(vm.offset, size)

    def test_constaddr_report_int(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 4, Opcodes.ADDR_VALTYPE_INT))
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_L2VALUE))

        vm, size = self.create(stack=struct.pack('<ii', 1, 2))
        report = Report(L2Value=42)
        vm.step(report)

        self.assertEqual(vm.get_stack(), struct.pack('<ii', 1, 2 + 42))
        self.assertEqual(vm.offset, size)

    def test_constaddr_report_float(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 4, Opcodes.ADDR_VALTYPE_FLOAT))
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_IMUX))

        vm, size = self.create(stack=struct.pack('<if', 1, 2.0))
        report = Report(IMUX=42.5)
        vm.step(report)

        self.assertEqual(vm.get_stack(), struct.pack('<if', 1, 2.0 + 42.5))
        self.assertEqual(vm.offset, size)

    def test_constaddr_regoff_int(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 4, Opcodes.ADDR_VALTYPE_INT))
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -8, Opcodes.ADDR_VALTYPE_INT))

        vm, size = self.create(stack=struct.pack('<iiii', 1, 2, 3, 4))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<iiii', 1, 5, 3, 4))
        self.assertEqual(vm.offset, size)

    def test_constaddr_regoff_float(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 4, Opcodes.ADDR_VALTYPE_FLOAT))
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -8, Opcodes.ADDR_VALTYPE_FLOAT))

        vm, size = self.create(stack=struct.pack('<iffi', 1, 2.5, 3.5, 4))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<iffi', 1, 6.0, 3.5, 4))
        self.assertEqual(vm.offset, size)

    # Regoff target

    def test_regoff_reg(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -8, Opcodes.ADDR_VALTYPE_INT))
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_TH))

        vm, size = self.create(stack=struct.pack('<iii', 1, 2, 3))
        vm.TH = 42
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<iii', 1, 2 + 42, 3))
        self.assertEqual(vm.offset, size)

    def test_regoff_report_int(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -8, Opcodes.ADDR_VALTYPE_INT))
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_L2VALUE))

        vm, size = self.create(stack=struct.pack('<iii', 1, 2, 3))
        vm.step(Report(L2Value=42))

        self.assertEqual(vm.get_stack(), struct.pack('<iii', 1, 2 + 42, 3))
        self.assertEqual(vm.offset, size)

    def test_regoff_report_float(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -8, Opcodes.ADDR_VALTYPE_FLOAT))
        self.add(Opcodes.make_addr_reg(Opcodes.REGINDEX_IMUY))

        vm, size = self.create(stack=struct.pack('<ifi', 1, 2.5, 3))
        vm.step(Report(IMUY=42.5))

        self.assertEqual(vm.get_stack(), struct.pack('<ifi', 1, 2.5 + 42.5, 3))
        self.assertEqual(vm.offset, size)

    def test_regoff_constaddr_int(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -8, Opcodes.ADDR_VALTYPE_INT))
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 4, Opcodes.ADDR_VALTYPE_INT))

        vm, size = self.create(stack=struct.pack('<iiii', 1, 2, 3, 4))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<iiii', 1, 2, 5, 4))
        self.assertEqual(vm.offset, size)

    def test_regoff_constaddr_float(self):
        self.add_opcode(Opcodes.OPCODE_VARIANT_A)
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_SP, -8, Opcodes.ADDR_VALTYPE_FLOAT))
        self.add(Opcodes.make_addr_regoff(Opcodes.REGINDEX_ZR, 4, Opcodes.ADDR_VALTYPE_FLOAT))

        vm, size = self.create(stack=struct.pack('<iffi', 1, 2.5, 3.5, 4))
        vm.step(Report())

        self.assertEqual(vm.get_stack(), struct.pack('<iffi', 1, 2.5, 6.0, 4))
        self.assertEqual(vm.offset, size)


if __name__ == '__main__':
    unittest.main()
