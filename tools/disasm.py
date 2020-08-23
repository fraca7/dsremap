#!/usr/bin/env python3

import os
import sys
import struct
import getopt

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src', 'ext'))
from dsrlib.compiler.opcodes import Opcodes
from vmwrapper import VM, Report


def read_bytecode(stream):
    magic, = struct.unpack('<H', stream.read(2))
    if magic != 0xCAFE:
        raise RuntimeError('Invalid magic 0x%04x' % magic)
    return read_configuration(stream)


def read_configuration(stream):
    conflen, = struct.unpack('<H', stream.read(2))
    if conflen == 0:
        return
    print('!! Configuration size: %d bytes' % conflen)

    while conflen:
        actionlen, = struct.unpack('<H', stream.read(2))
        print('!! Action size: %d bytes' % actionlen)
        stacksize, = struct.unpack('<H', stream.read(2))
        print('!! Stack size: %d bytes' % stacksize)
        bytecode = stream.read(actionlen - 2)
        disasm(bytecode)
        conflen -= actionlen + 2

    read_configuration(stream)
    return stacksize, bytecode


def disasm_one(bytecode, offset):
    label = '0x%04x' % offset
    opcode = bytecode[offset]
    offset += 1
    maintype = Opcodes.opcode_type(opcode)
    subtype = Opcodes.opcode_subtype(opcode)
    variant = Opcodes.opcode_variant(opcode)

    if maintype == Opcodes.OPCODE_TYPE_BINARY:
        dst = Opcodes.make_addr_str(bytecode[offset:])
        offset += dst.size

        if variant == Opcodes.OPCODE_VARIANT_C:
            if dst.type == 'i':
                src, = struct.unpack('<h', bytecode[offset:offset+2])
                offset += 2
            else:
                src, = struct.unpack('<f', bytecode[offset:offset+4])
                offset += 4
        elif variant == Opcodes.OPCODE_VARIANT_A:
            src = Opcodes.make_addr_str(bytecode[offset:])
            offset += src.size
        else:
            raise NotImplementedError

        name = {
            Opcodes.OPCODE_SUBTYPE_BINARY_ADD: 'ADD',
            Opcodes.OPCODE_SUBTYPE_BINARY_SUB: 'SUB',
            Opcodes.OPCODE_SUBTYPE_BINARY_MUL: 'MUL',
            Opcodes.OPCODE_SUBTYPE_BINARY_DIV: 'DIV',
            Opcodes.OPCODE_SUBTYPE_BINARY_CLT: 'CLT',
            Opcodes.OPCODE_SUBTYPE_BINARY_CGT: 'CGT',
            Opcodes.OPCODE_SUBTYPE_BINARY_CLTE: 'CLTE',
            Opcodes.OPCODE_SUBTYPE_BINARY_CGTE: 'CGTE',
            Opcodes.OPCODE_SUBTYPE_BINARY_CE: 'CE',
            Opcodes.OPCODE_SUBTYPE_BINARY_CN: 'CN',
            Opcodes.OPCODE_SUBTYPE_BINARY_AND: 'AND',
            Opcodes.OPCODE_SUBTYPE_BINARY_OR: 'OR',
            Opcodes.OPCODE_SUBTYPE_BINARY_LOAD: 'LOAD',
            Opcodes.OPCODE_SUBTYPE_BINARY_CAST: 'CAST',
            }[subtype]

        print('%s %s\t%s, %s' % (label, name, dst, src))
    elif maintype == Opcodes.OPCODE_TYPE_UNARY:
        dst = Opcodes.make_addr_str(bytecode[offset:])
        offset += dst.size

        name = {
            Opcodes.OPCODE_SUBTYPE_UNARY_NOT: 'NOT',
            Opcodes.OPCODE_SUBTYPE_UNARY_NEG: 'NEG',
            }[subtype]
        print('%s %s\t%s' % (label, name, dst))
    elif maintype == Opcodes.OPCODE_TYPE_STACK:
        if subtype == Opcodes.OPCODE_SUBTYPE_STACK_PUSHI:
            val, = struct.unpack('<h', bytecode[offset:offset+2])
            offset += 2
            print('%s PUSH\t%s' % (label, val))
        elif subtype == Opcodes.OPCODE_SUBTYPE_STACK_PUSHF:
            val, = struct.unpack('<f', bytecode[offset:offset+4])
            offset += 4
            print('%s PUSH\t%s' % (label, val))
        elif subtype == Opcodes.OPCODE_SUBTYPE_STACK_PUSH:
            src = Opcodes.make_addr_str(bytecode[offset:])
            offset += src.size
            print('%s PUSH\t%s' % (label, src))
        elif subtype == Opcodes.OPCODE_SUBTYPE_STACK_POP:
            src = Opcodes.make_addr_str(bytecode[offset:])
            offset += src.size
            print('%s POP\t%s' % (label, src))
    elif maintype == Opcodes.OPCODE_TYPE_FLOW:
        if subtype == Opcodes.OPCODE_SUBTYPE_FLOW_JZ:
            if variant == Opcodes.OPCODE_VARIANT_A:
                cond = Opcodes.make_addr_str(bytecode[offset:])
                offset += cond.size
            elif variant == Opcode.OPCODE_VARIANT_CI:
                cond = struct.unpack('<i', bytecode[offset:offset+2])
                offset += 2
            elif variant == Opcode.OPCODE_VARIANT_CF:
                cond = struct.unpack('<f', bytecode[offset:offset+4])
                offset += 4
            else:
                raise NotImplementedError
            jumpto, = struct.unpack('<H', bytecode[offset:offset+2])
            offset += 2
            print('%s JZ\t%s, 0x%04x' % (label, cond, jumpto))
        elif subtype == Opcodes.OPCODE_SUBTYPE_FLOW_JUMP:
            jumpto, = struct.unpack('<H', bytecode[offset:offset+2])
            offset += 2
            print('%s JUMP\t0x%04x' % (label, jumpto))
        elif subtype == Opcodes.OPCODE_SUBTYPE_FLOW_YIELD:
            print('%s YIELD' % label)
        elif subtype == Opcodes.OPCODE_SUBTYPE_FLOW_RET:
            print('%s RET' % label)
        elif subtype == Opcodes.OPCODE_SUBTYPE_FLOW_CALL:
            jumpto, = struct.unpack('<H', bytecode[offset:offset+2])
            offset += 2
            print('%s CALL\t0x%04x' % (label, jumpto))
        else:
            raise NotImplementedError
    else:
        raise NotImplementedError

    return offset


def disasm(bytecode):
    offset = 0
    while offset < len(bytecode):
        offset = disasm_one(bytecode, offset)


def execute(stacksize, bytecode):
    vm = VM(bytecode=bytecode, stacksize=stacksize)
    while True:
        disasm_one(bytecode, vm.offset)
        print('Stack: %s' % vm.get_stack())
        input('...')
        vm.step(Report())


def main(argv):
    opts, args = getopt.getopt(argv, 'e', ['execute'])

    exe = False
    for opt, val in opts:
        if opt in ('-e', '--execute'):
            exe = True

    for name in args:
        with open(name, 'rb') as fileobj:
            print('!! File: %s' % name)
            stacksize, bytecode = read_bytecode(fileobj)
            if exe:
                execute(stacksize, bytecode)


if __name__ == '__main__':
    main(sys.argv[1:])
