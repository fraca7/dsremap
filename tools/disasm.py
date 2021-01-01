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
    prev_offset = offset
    offset += 1
    maintype = Opcodes.opcode_type(opcode)
    subtype = Opcodes.opcode_subtype(opcode)
    variant = Opcodes.opcode_variant(opcode)

    if maintype == Opcodes.OPCODE_TYPE_BINARY:
        dst = Opcodes.make_addr_str(bytecode[offset:])
        offset += dst.size

        if variant == Opcodes.OPCODE_VARIANT_C:
            src, = struct.unpack('<%s' % dst.type, bytecode[offset:offset+4])
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

        instr = '%- 10s %s, %s' % (name, dst, src)
    elif maintype == Opcodes.OPCODE_TYPE_UNARY:
        dst = Opcodes.make_addr_str(bytecode[offset:])
        offset += dst.size

        name = {
            Opcodes.OPCODE_SUBTYPE_UNARY_NOT: 'NOT',
            Opcodes.OPCODE_SUBTYPE_UNARY_NEG: 'NEG',
            }[subtype]
        instr = '%- 10s %s' % (name, dst)
    elif maintype == Opcodes.OPCODE_TYPE_STACK:
        if subtype == Opcodes.OPCODE_SUBTYPE_STACK_PUSHI:
            val, = struct.unpack('<i', bytecode[offset:offset+4])
            offset += 4
            instr = '%- 10s %s' % ('PUSH', val)
        elif subtype == Opcodes.OPCODE_SUBTYPE_STACK_PUSHF:
            val, = struct.unpack('<f', bytecode[offset:offset+4])
            offset += 4
            instr = '%- 10s %s' % ('PUSH', val)
        elif subtype == Opcodes.OPCODE_SUBTYPE_STACK_PUSH:
            src = Opcodes.make_addr_str(bytecode[offset:])
            offset += src.size
            instr = '%- 10s %s' % ('PUSH', src)
        elif subtype == Opcodes.OPCODE_SUBTYPE_STACK_POP:
            src = Opcodes.make_addr_str(bytecode[offset:])
            offset += src.size
            instr = '%- 10s %s' % ('POP', src)
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
            instr = '%- 10s %s, 0x%04x' % ('JZ', cond, jumpto)
        elif subtype == Opcodes.OPCODE_SUBTYPE_FLOW_JUMP:
            jumpto, = struct.unpack('<H', bytecode[offset:offset+2])
            offset += 2
            instr = '%- 10s 0x%04x' % ('JUMP', jumpto)
        elif subtype == Opcodes.OPCODE_SUBTYPE_FLOW_YIELD:
            instr = '%- 10s' % 'YIELD'
        elif subtype == Opcodes.OPCODE_SUBTYPE_FLOW_RET:
            instr = '%- 10s' % 'RET'
        elif subtype == Opcodes.OPCODE_SUBTYPE_FLOW_CALL:
            jumpto, = struct.unpack('<H', bytecode[offset:offset+2])
            offset += 2
            instr = '%- 10s 0x%04x' % ('CALL', jumpto)
        else:
            raise NotImplementedError
    else:
        raise NotImplementedError

    print('%s %- 40s %s' % (label, instr, ' '.join(['%02X' % val for val in bytecode[prev_offset:offset]])))

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
