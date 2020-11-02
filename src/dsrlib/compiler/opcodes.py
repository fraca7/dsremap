#!/usr/bin/env python3

import sys
import codecs
import os
import inspect
import struct
import collections

class AddrRepr(collections.namedtuple('AddrRepr', ['str', 'size', 'type'])):
    def __str__(self):
        return self.str


class Opcodes:
    # opcode type: 2 bits.
    OPCODE_TYPE_BINARY = 0x00
    OPCODE_TYPE_UNARY = 0x01
    OPCODE_TYPE_FLOW = 0x02
    OPCODE_TYPE_STACK = 0x03

    # opcode subtype: 4 bits, depending op main opcode type
    OPCODE_SUBTYPE_BINARY_ADD = 0x00
    OPCODE_SUBTYPE_BINARY_SUB = 0x01
    OPCODE_SUBTYPE_BINARY_MUL = 0x02
    OPCODE_SUBTYPE_BINARY_DIV = 0x03
    OPCODE_SUBTYPE_BINARY_CLT = 0x04
    OPCODE_SUBTYPE_BINARY_CGT = 0x05
    OPCODE_SUBTYPE_BINARY_CLTE = 0x06
    OPCODE_SUBTYPE_BINARY_CGTE = 0x07
    OPCODE_SUBTYPE_BINARY_CE = 0x08
    OPCODE_SUBTYPE_BINARY_CN = 0x09
    OPCODE_SUBTYPE_BINARY_AND = 0x0A
    OPCODE_SUBTYPE_BINARY_OR = 0x0B
    OPCODE_SUBTYPE_BINARY_LOAD = 0x0C
    OPCODE_SUBTYPE_BINARY_CAST = 0x0D

    OPCODE_SUBTYPE_UNARY_NEG = 0x00
    OPCODE_SUBTYPE_UNARY_NOT = 0x01

    OPCODE_SUBTYPE_FLOW_CALL = 0x00
    OPCODE_SUBTYPE_FLOW_RET = 0x01
    OPCODE_SUBTYPE_FLOW_YIELD = 0x02
    OPCODE_SUBTYPE_FLOW_JUMP = 0x03
    OPCODE_SUBTYPE_FLOW_JZ = 0x04

    OPCODE_SUBTYPE_STACK_PUSHI = 0x00
    OPCODE_SUBTYPE_STACK_PUSHF = 0x01
    OPCODE_SUBTYPE_STACK_PUSH = 0x02
    OPCODE_SUBTYPE_STACK_POP = 0x03

    # Opcode variant: 2 bit. This identifies the type of address for
    # the second (binary) or single (unary) operand. The first operand
    # for a binary is also the destination, and always a register.

    # Address or constant
    OPCODE_VARIANT_A = 0x00
    OPCODE_VARIANT_C = 0x01

    # JZ may have an A variant, or 2 different C variants
    OPCODE_VARIANT_CI = 0x01
    OPCODE_VARIANT_CF = 0x02

    # Address type: 2 bits. Either a register or a memory location
    # based on register+offset
    ADDR_TYPE_REG = 0x00
    ADDR_TYPE_REGOFF = 0x01

    # Value type: 1 bit
    ADDR_VALTYPE_INT = 0x00
    ADDR_VALTYPE_FLOAT = 0x01

    # Registers + report registers
    REGINDEX_SP = 0x00
    REGINDEX_TH = 0x01
    REGINDEX_ZR = 0x02

    REGINDEX_LPADX = 0x03
    REGINDEX_LPADY = 0x04
    REGINDEX_RPADX = 0x05
    REGINDEX_RPADY = 0x06
    REGINDEX_HAT = 0x07
    REGINDEX_SQUARE = 0x08
    REGINDEX_CROSS = 0x09
    REGINDEX_CIRCLE = 0x0A
    REGINDEX_TRIANGLE = 0x0B
    REGINDEX_L1 = 0x0C
    REGINDEX_L2 = 0x0D
    REGINDEX_R1 = 0x0E
    REGINDEX_R2 = 0x0F
    REGINDEX_SHARE = 0x10
    REGINDEX_OPTIONS = 0x11
    REGINDEX_L3 = 0x12
    REGINDEX_R3 = 0x13
    REGINDEX_PS = 0x14
    REGINDEX_TPAD = 0x15
    REGINDEX_L2VALUE = 0x16
    REGINDEX_R2VALUE = 0x17
    REGINDEX_IMUX = 0x18
    REGINDEX_IMUY = 0x19
    REGINDEX_IMUZ = 0x1A
    REGINDEX_DELTA = 0x1B
    REGINDEX_ACCELX = 0x1C
    REGINDEX_ACCELY = 0x1D
    REGINDEX_ACCELZ = 0x1E

    @classmethod
    def _register_map(cls):
        return {
            cls.REGINDEX_ZR: '%ZR',
            cls.REGINDEX_TH: '%TH',
            cls.REGINDEX_SP: '%SP',
            cls.REGINDEX_LPADX: 'LPadX',
            cls.REGINDEX_LPADY: 'LPadY',
            cls.REGINDEX_RPADX: 'RPadX',
            cls.REGINDEX_RPADY: 'RPadY',
            cls.REGINDEX_HAT: 'Hat',
            cls.REGINDEX_SQUARE: 'Square',
            cls.REGINDEX_CROSS: 'Cross',
            cls.REGINDEX_CIRCLE: 'Circle',
            cls.REGINDEX_TRIANGLE: 'Triangle',
            cls.REGINDEX_L1: 'L1',
            cls.REGINDEX_L2: 'L2',
            cls.REGINDEX_R1: 'R1',
            cls.REGINDEX_R2: 'R2',
            cls.REGINDEX_SHARE: 'Share',
            cls.REGINDEX_OPTIONS: 'Options',
            cls.REGINDEX_L3: 'L3',
            cls.REGINDEX_R3: 'R3',
            cls.REGINDEX_PS: 'PS',
            cls.REGINDEX_TPAD: 'TPad',
            cls.REGINDEX_L2VALUE: 'L2Value',
            cls.REGINDEX_R2VALUE: 'R2Value',
            cls.REGINDEX_IMUX: 'IMUX',
            cls.REGINDEX_IMUY: 'IMUY',
            cls.REGINDEX_IMUZ: 'IMUZ',
            cls.REGINDEX_DELTA: 'DELTA',
            }

    @classmethod
    def register_name(cls, index):
        return cls._register_map()[index]

    @classmethod
    def register_index(cls, name):
        for key, value in cls._register_map().items():
            if value == name:
                return key
        raise NameError(name)

    @classmethod
    def type_suffix(cls, type_):
        return {
            cls.ADDR_VALTYPE_INT: 'i',
            cls.ADDR_VALTYPE_FLOAT: 'f',
            }[type_]

    @classmethod
    def type_value(cls, name):
        return {
            'int': cls.ADDR_VALTYPE_INT,
            'float': cls.ADDR_VALTYPE_FLOAT,
            }.get(name, None)

    @classmethod
    def make_addr_reg(cls, index):
        return struct.pack('>B', (cls.ADDR_TYPE_REG << 6) | index)

    @classmethod
    def make_addr_regoff(cls, index, offset, valtype):
        assert index >= 0 and index <= cls.REGINDEX_ZR, index
        assert offset >= (0 if index == cls.REGINDEX_ZR else -(1<<10)) and offset < (1<<10), offset
        if offset < 0:
            offset = -offset | (1<<10)
        return struct.pack('>H', (cls.ADDR_TYPE_REGOFF << 14) | (index << 12) | (valtype << 11) | offset)

    @classmethod
    def make_addr_str(cls, addr):
        value = addr[0]
        type_ = value >> 6
        if type_ == cls.ADDR_TYPE_REG:
            index = value & 0b00111111
            return AddrRepr(cls.register_name(index), 1, 'i')
        if type_ == cls.ADDR_TYPE_REGOFF:
            value = (value << 8) | addr[1]
            index = (value & 0b0011000000000000) >> 12
            valtype = (value & 0b0000100000000000) >> 11
            offset = value & 0b0000011111111111
            if offset & 0b0000010000000000 != 0:
                offset = -(offset & 0b0000001111111111)
            type_ = ['i', 'f'][valtype]
            return AddrRepr('[%s%s]%s' % (cls.register_name(index), '+%d' % offset if offset >= 0 else '%d' % offset, type_), 2, type_)
        raise RuntimeError('Invalid type %d' % type_)

    @staticmethod
    def make_opcode(maintype, subtype, variant=0):
        return bytes([(maintype << 6) | (subtype << 2) | variant])

    @staticmethod
    def opcode_type(opcode):
        return opcode >> 6

    @staticmethod
    def opcode_subtype(opcode):
        return (opcode >> 2) & 0b1111

    @staticmethod
    def opcode_variant(opcode):
        return opcode & 0b11


def export(filename):
    name, _ = os.path.splitext(os.path.basename(filename))
    with codecs.getwriter('utf-8')(open(filename, 'wb')) as dst:
        dst.write('#ifndef _%s_H_\n' % name.upper())
        dst.write('#define _%s_H_\n' % name.upper())
        for name, value in inspect.getmembers(Opcodes, lambda x: isinstance(x, int)):
            dst.write('#define %s 0x%02x\n' % (name, value))
        dst.write('#endif /* _%s_H_ */\n' % name.upper())


if __name__ == '__main__':
    export(sys.argv[1])
