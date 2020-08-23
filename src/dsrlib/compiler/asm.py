#!/usr/bin/env python3

import struct

from .opcodes import Opcodes


class AddrVisitor:
    def visit(self, addr):
        return getattr(self, 'visit%s' % addr.__class__.__name__)(addr)

    def visitRegoffAddr(self, addr):
        raise NotImplementedError

    def visitRegAddr(self, addr):
        raise NotImplementedError

    def visitConstAddr(self, addr):
        raise NotImplementedError


class OpcodeVariantVisitor(AddrVisitor):
    def visitRegoffAddr(self, _):
        return Opcodes.OPCODE_VARIANT_A

    def visitRegAddr(self, _):
        return Opcodes.OPCODE_VARIANT_A

    def visitConstAddr(self, _):
        return Opcodes.OPCODE_VARIANT_C


class AddrSuffixVisitor(OpcodeVariantVisitor):
    def visit(self, addr):
        return {
            Opcodes.OPCODE_VARIANT_A: 'A',
            Opcodes.OPCODE_VARIANT_C: 'C',
            }[super().visit(addr)]


class AddrOpcodeVisitor(AddrVisitor):
    def visitRegoffAddr(self, addr):
        return Opcodes.make_addr_regoff(addr.reg, addr.offset, addr.type)

    def visitRegAddr(self, addr):
        return Opcodes.make_addr_reg(addr.reg)

    def visitConstAddr(self, addr):
        if addr.type() == 'int':
            return struct.pack('<h', addr.value())
        if addr.type() == 'float':
            return struct.pack('<f', addr.value())
        raise RuntimeError('Unknown type "%s"' % addr.type())


class Instruction:
    maintype = None
    subtype = None

    def __str__(self):
        raise NotImplementedError

    def write(self, offsets, stream):
        raise NotImplementedError


class BinaryInstruction(Instruction):
    maintype = Opcodes.OPCODE_TYPE_BINARY
    subtype = None

    def __init__(self, dst, src):
        super().__init__()
        self.src = src
        self.dst = dst

    def __str__(self):
        return '\t%s%s\t%s, %s' % (self.__class__.__name__.upper(), AddrSuffixVisitor().visit(self.src), str(self.dst), str(self.src))

    def write(self, offsets, stream):
        stream.write(Opcodes.make_opcode(self.maintype, self.subtype, OpcodeVariantVisitor().visit(self.src)))
        stream.write(AddrOpcodeVisitor().visit(self.dst))
        stream.write(AddrOpcodeVisitor().visit(self.src))


class Add(BinaryInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_BINARY_ADD


class Sub(BinaryInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_BINARY_SUB


class Mul(BinaryInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_BINARY_MUL


class Div(BinaryInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_BINARY_DIV


class CLT(BinaryInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_BINARY_CLT


class CGT(BinaryInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_BINARY_CGT


class CLTE(BinaryInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_BINARY_CLTE


class CGTE(BinaryInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_BINARY_CGTE


class CE(BinaryInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_BINARY_CE


class CN(BinaryInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_BINARY_CN


class And(BinaryInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_BINARY_AND


class Or(BinaryInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_BINARY_OR


class Load(BinaryInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_BINARY_LOAD


class Cast(BinaryInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_BINARY_CAST


class UnaryInstruction(Instruction):
    maintype = Opcodes.OPCODE_TYPE_UNARY
    subtype = None

    def __init__(self, dst):
        super().__init__()
        self.dst = dst

    def __str__(self):
        return '\t%s\t%s' % (self.__class__.__name__.upper(), str(self.dst))

    def write(self, offsets, stream):
        stream.write(Opcodes.make_opcode(self.maintype, self.subtype))
        stream.write(AddrOpcodeVisitor().visit(self.dst))


class Neg(UnaryInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_UNARY_NEG


class Not(UnaryInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_UNARY_NOT


class FlowInstruction(Instruction):
    maintype = Opcodes.OPCODE_TYPE_FLOW
    subtype = None

    def __str__(self):
        raise NotImplementedError

    def write(self, offsets, stream):
        stream.write(Opcodes.make_opcode(self.maintype, self.subtype))


class Call(FlowInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_FLOW_CALL

    def __init__(self, label):
        super().__init__()
        self.label = label

    def __str__(self):
        return '\tCALL\t%s' % str(self.label)

    def write(self, offsets, stream):
        super().write(offsets, stream)
        offsets.setdefault(self.label, []).append(stream.tell())
        stream.write(struct.pack('<H', 0))


class Ret(FlowInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_FLOW_RET

    def __str__(self):
        return '\tRET'


class Yield(FlowInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_FLOW_YIELD

    def __str__(self):
        return '\tYIELD'


class Jump(FlowInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_FLOW_JUMP

    def __init__(self, label):
        super().__init__()
        self.label = label

    def __str__(self):
        return '\tJUMP\t%s' % str(self.label)

    def write(self, offsets, stream):
        super().write(offsets, stream)
        offsets.setdefault(self.label, []).append(stream.tell())
        stream.write(struct.pack('<H', 0))


class JZVariantVisitor(AddrVisitor):
    def visitRegoffAddr(self, _):
        return Opcodes.OPCODE_VARIANT_A

    def visitRegAddr(self, _):
        return Opcodes.OPCODE_VARIANT_A

    def visitConstAddr(self, addr):
        return Opcodes.OPCODE_VARIANT_CI if addr.type() == 'int' else Opcodes.OPCODE_VARIANT_CF


class JZ(FlowInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_FLOW_JZ

    def __init__(self, cond, label):
        super().__init__()
        self.cond = cond
        self.label = label

    def __str__(self):
        name = {
            Opcodes.OPCODE_VARIANT_A: 'JZ',
            Opcodes.OPCODE_VARIANT_CI: 'JZI',
            Opcodes.OPCODE_VARIANT_CF: 'JZF',
            }[JZVariantVisitor().visit(self.cond)]
        return '\t%s\t%s, %s' % (name, str(self.cond), str(self.label))

    def write(self, offsets, stream):
        # Do NOT call parent
        stream.write(Opcodes.make_opcode(self.maintype, self.subtype, JZVariantVisitor().visit(self.cond)))
        stream.write(AddrOpcodeVisitor().visit(self.cond))
        offsets.setdefault(self.label, []).append(stream.tell())
        stream.write(struct.pack('<H', 0))


class StackInstruction(Instruction):
    maintype = Opcodes.OPCODE_TYPE_STACK
    subtype = None

    def __str__(self):
        raise NotImplementedError

    def write(self, offsets, stream):
        stream.write(Opcodes.make_opcode(self.maintype, self.subtype))


class PushTypeVisitor(AddrVisitor):
    def visitRegoffAddr(self, _):
        return Opcodes.OPCODE_SUBTYPE_STACK_PUSH

    def visitRegAddr(self, _):
        return Opcodes.OPCODE_SUBTYPE_STACK_PUSH

    def visitConstAddr(self, addr):
        return Opcodes.OPCODE_SUBTYPE_STACK_PUSHI if addr.type() == 'int' else Opcodes.OPCODE_SUBTYPE_STACK_PUSHF


class Push(StackInstruction):
    def __init__(self, value):
        super().__init__()
        self.value = value

    @property
    def subtype(self):
        return PushTypeVisitor().visit(self.value)

    def write(self, offsets, stream):
        super().write(offsets, stream)
        stream.write(AddrOpcodeVisitor().visit(self.value))

    def __str__(self):
        name = {
            Opcodes.OPCODE_SUBTYPE_STACK_PUSH: 'PUSH',
            Opcodes.OPCODE_SUBTYPE_STACK_PUSHI: 'PUSHI',
            Opcodes.OPCODE_SUBTYPE_STACK_PUSHF: 'PUSHF',
            }[PushTypeVisitor().visit(self.value)]
        return '\t%s\t%s' % (name, str(self.value))


class Pop(StackInstruction):
    subtype = Opcodes.OPCODE_SUBTYPE_STACK_POP

    def __init__(self, addr):
        super().__init__()
        self.addr = addr

    def write(self, offsets, stream):
        super().write(offsets, stream)
        stream.write(AddrOpcodeVisitor().visit(self.addr))

    def __str__(self):
        return '\tPOP\t%s' % str(self.addr)
