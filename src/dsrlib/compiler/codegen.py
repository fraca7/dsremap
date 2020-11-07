#!/usr/bin/env python3

import collections

from .symbols import SymbolScope
from .ast import ASTVisitor, ASTScopedVisitorMixin
from .mtypes import MethodType
from .opcodes import Opcodes
from . import asm

# Function activation block:
# arguments...
# retval
# retaddr
# locals


class StackSize(ASTScopedVisitorMixin, ASTVisitor):
    def visitEmptyNode(self, node):
        return 0

    def visitConstantNode(self, node):
        return 0

    def visitIdentifierNode(self, node):
        return 0

    def visitPostfixUnaryNode(self, node):
        return self.visit(node.target)

    def visitPrefixUnaryNode(self, node):
        return self.visit(node.target)

    def visitUnaryNode(self, node):
        return self.visit(node.expr)

    def visitBinaryNode(self, node):
        return self.visit(node.op1) + self.visit(node.op2)

    def visitCallNode(self, node):
        ftype = node.target.type()
        # Arguments, retval, retaddr
        size = ftype.argsize() + ftype.savesize() + node.type().size + 2
        if isinstance(ftype, MethodType):
            symbols = node.target.target.type().symbols
        else:
            symbols = self.currentTable()

        func = symbols.get(node.target.name).value
        size += CalleeStackSize().visit(func)

        return size

    def visitAccessNode(self, node):
        return 0

    def visitVariableNode(self, node):
        return self.visit(node.expr)

    def visitMemberNode(self, node):
        return 0

    def visitArgumentNode(self, node):
        return 0

    def visitStructNode(self, node):
        return 0

    def visitStateNode(self, node):
        return node.symbols.size() + max(self.visit(node.init), self.visit(node.main))

    def visitIfNode(self, node):
        return self.visit(node.expr) + max(self.visit(node.yes), self.visit(node.no))

    def visitWhileNode(self, node):
        return self.visit(node.body)

    def visitAssignmentNode(self, node):
        return 0

    def visitContinueNode(self, node):
        return 0

    def visitBreakNode(self, node):
        return 0

    def visitYieldNode(self, node):
        return 0

    def visitGoNode(self, node):
        return 0

    def visitReturnNode(self, node):
        return 0

    def visitFunctionNode(self, node):
        return 0

    def visitMethodNode(self, node):
        return 0

    def visitStateMethodNode(self, node):
        return node.symbols.size() + self.visit(node.body)

    def visitCompoundStatementNode(self, node):
        size = node.symbols.size()
        if node.statements:
            size += max([self.visit(statement) for statement in node.statements])
        return size


class CalleeStackSize(StackSize):
    def visitFunctionNode(self, node):
        return StackSize().visit(node.body)

    def visitMethodNode(self, node):
        return StackSize().visit(node.body)


class RegoffAddr(collections.namedtuple('RegoffAddr', ['reg', 'offset', 'type'])):
    def __str__(self):
        return '[%s%s%s]%s' % (Opcodes.register_name(self.reg), '+' if self.offset >= 0 else '', self.offset, 'i' if self.type == Opcodes.ADDR_VALTYPE_INT else 'f')


class RegAddr(collections.namedtuple('RegAddr', ['reg'])):
    def __str__(self):
        return Opcodes.register_name(self.reg)


class ConstAddr:
    def __init__(self, value):
        self._value = value

    def __str__(self):
        return str(self._value)

    def type(self):
        return 'float' if isinstance(self._value, float) else 'int'

    def value(self):
        return self._value


class AddressGenerator:
    def __init__(self):
        self._offset = 0

    def addArg(self, arg):
        self._offset += arg.var.type().size

    def resetArgs(self):
        self._offset = 0

    def generate(self, symbols, value):
        return getattr(self, 'generate%s' % value.__class__.__name__)(symbols, value)

    def generateConst(self, symbols, const): # pylint: disable=W0613
        return ConstAddr(const.value)

    def generateTempVar(self, symbols, var):
        return self._generateVar(symbols, var)

    def generateVariableNode(self, symbols, var):
        return self._generateVar(symbols, var)

    def generateRetval(self, symbols, var): # pylint: disable=W0613
        # Caller context
        return RegoffAddr(Opcodes.REGINDEX_SP, var.function.argsize(), Opcodes.type_value(var.function.rettype.name))

    def generateMember(self, symbols, var):
        reg, offset, _ = self._generateVar(symbols, var.root)
        root = var.root.type()
        for field in var.fields:
            offset += root.symbols.offset(field)
            root = root.memberType(field)
        return RegoffAddr(reg, offset, Opcodes.type_value(root.name))

    def generateMemberNode(self, symbols, var):
        return self._generateVar(symbols, var)

    def generateArgumentNode(self, symbols, var):
        return self._generateVar(symbols, var)

    def generateBuiltinVariable(self, symbols, var): # pylint: disable=W0613
        return RegAddr(Opcodes.register_index(var.name))

    def _generateVar(self, symbols, var):
        offset = 0
        while True:
            type_ = symbols.type()

            if symbols.has(var.name, recurse=False):
                if type_ == SymbolScope.GLOBAL:
                    reg, offset = Opcodes.REGINDEX_ZR, symbols.offset(var.name)
                    break
                if type_ in (SymbolScope.LOCAL, SymbolScope.INHERITED):
                    offset -= symbols.size()
                    offset += symbols.offset(var.name)
                    offset -= self._offset
                    reg = Opcodes.REGINDEX_SP
                    break
                if type_ == SymbolScope.THIS:
                    reg, offset = Opcodes.REGINDEX_TH, symbols.offset(var.name)
                    break
                if type_ == SymbolScope.ARGS:
                    offset -= 2 # ret addr
                    # retval size is included in the symbol table size
                    offset -= symbols.size()
                    offset += symbols.offset(var.name)
                    offset -= self._offset
                    reg = Opcodes.REGINDEX_SP
                    break
            else:
                offset -= symbols.size()

            symbols = symbols.parent()
        return RegoffAddr(reg, offset, Opcodes.type_value(var.type().name))


class CodeGenerator:
    def __init__(self):
        self._gen = AddressGenerator()
        self._instructions = []

    def generate(self, ops):
        for op in ops:
            getattr(self, 'generate%s' % op.__class__.__name__)(op)
        return self._instructions

    def _generateBinary(self, cls, dst, src):
        self._instructions.append(cls(dst, src))

    def generateLineSpec(self, line):
        pass

    def generateLabel(self, label):
        self._instructions.append(label)

    def generateStateEnterLabel(self, label):
        self.generateLabel(label)
        size = label.state.symbols.size()
        if size:
            self._generateBinary(asm.Add, RegAddr(Opcodes.REGINDEX_SP), ConstAddr(size))

    def generateCallableLabel(self, label):
        self.generateLabel(label)

    def generateCompoundStartLabel(self, label):
        self.generateLabel(label)
        size = label.symbols.size()
        if size:
            self._generateBinary(asm.Add, RegAddr(Opcodes.REGINDEX_SP), ConstAddr(size))

    def generateCompoundEndLabel(self, label):
        self.generateLabel(label)
        size = label.symbols.size()
        if size:
            self._generateBinary(asm.Sub, RegAddr(Opcodes.REGINDEX_SP), ConstAddr(size))

    def generateArgument(self, arg):
        var = self._gen.generate(arg.symbols, arg.var)
        self._instructions.append(asm.Push(var))
        self._gen.addArg(arg)

    def generateFunctionCall(self, call):
        size = call.function.rettype.size
        if size:
            self._generateBinary(asm.Add, RegAddr(Opcodes.REGINDEX_SP), ConstAddr(size))
        self._instructions.append(asm.Call(call.label))
        size += call.function.type().argsize()
        if size:
            self._generateBinary(asm.Sub, RegAddr(Opcodes.REGINDEX_SP), ConstAddr(size))
        self._gen.resetArgs()

    def generateMethodCall(self, call):
        size = call.method.rettype.size
        if size:
            self._generateBinary(asm.Add, RegAddr(Opcodes.REGINDEX_SP), ConstAddr(size))
        self._instructions.append(asm.Push(RegAddr(Opcodes.REGINDEX_TH)))

        addr = self._gen.generate(call.symbols, call.var)
        if addr.reg == Opcodes.REGINDEX_ZR:
            self._generateBinary(asm.Load, RegAddr(Opcodes.REGINDEX_TH), ConstAddr(addr.offset))
        elif addr.reg == Opcodes.REGINDEX_SP:
            self._generateBinary(asm.Load, RegAddr(Opcodes.REGINDEX_TH), RegAddr(Opcodes.REGINDEX_SP))
            self._generateBinary(asm.Sub, RegAddr(Opcodes.REGINDEX_TH), ConstAddr((4 - addr.offset + size)))
        elif addr.reg == Opcodes.REGINDEX_TH:
            self._generateBinary(asm.Add, RegAddr(Opcodes.REGINDEX_TH), ConstAddr(addr.offset))
        else:
            raise RuntimeError('wat')

        self._instructions.append(asm.Call(call.label))
        self._instructions.append(asm.Pop(RegAddr(Opcodes.REGINDEX_TH)))

        size += call.method.type().argsize()
        if size:
            self._generateBinary(asm.Sub, RegAddr(Opcodes.REGINDEX_SP), ConstAddr(size))
        self._gen.resetArgs()

    def generateAssignment(self, op):
        self._generateBinary(asm.Load, self._gen.generate(op.symbols, op.dst), self._gen.generate(op.symbols, op.src))

    def generateReturn(self, op):
        size = 0
        symbols = op.symbols
        while symbols.type() != SymbolScope.ARGS:
            size += symbols.size()
            symbols = symbols.parent()

        if op.var is not None:
            dst = RegoffAddr(Opcodes.REGINDEX_SP, -(2 + op.var.type().size + op.func.type().savesize() + size), Opcodes.type_value(op.var.type().name))
            src = self._gen.generate(op.symbols, op.var)
            self._generateBinary(asm.Load, dst, src)

        if size:
            self._generateBinary(asm.Sub, RegAddr(Opcodes.REGINDEX_SP), ConstAddr(size))

        self._instructions.append(asm.Ret())

    def generateYield(self, op): # pylint: disable=W0613
        self._instructions.append(asm.Yield())

    def generateJump(self, op):
        self._instructions.append(asm.Jump(op.label))

    def generateUnaryOp(self, op):
        dst = self._gen.generate(op.symbols, op.dst)
        if op.op in ('casti', 'castf'):
            self._generateBinary(asm.Cast, dst, self._gen.generate(op.symbols, op.op1))
        else:
            self._generateBinary(asm.Load, dst, self._gen.generate(op.symbols, op.op1))
            if op.op == '-':
                self._instructions.append(asm.Neg(dst))
            elif op.op == '!':
                self._instructions.append(asm.Not(dst))
            else:
                raise RuntimeError(op.op)

    def generateBinaryOp(self, op):
        cmd = {
            '+': asm.Add,
            '-': asm.Sub,
            '*': asm.Mul,
            '/': asm.Div,
            '<': asm.CLT,
            '>': asm.CGT,
            '<=': asm.CLTE,
            '>=': asm.CGTE,
            '==': asm.CE,
            '!=': asm.CN,
            '&&': asm.And,
            '||': asm.Or,
            }[op.op]

        dst = self._gen.generate(op.symbols, op.dst)
        op2 = self._gen.generate(op.symbols, op.op2)
        if op.dst != op.op1:
            op1 = self._gen.generate(op.symbols, op.op1)
            self._generateBinary(asm.Load, dst, op1)
        self._generateBinary(cmd, dst, op2)

    def generateIfFalse(self, op):
        cond = self._gen.generate(op.symbols, op.cond)
        self._instructions.append(asm.JZ(cond, op.label))

    def generateGo(self, op):
        symbols = op.symbols
        while symbols.type() != SymbolScope.GLOBAL:
            symbols = symbols.parent()
        self._generateBinary(asm.Load, RegAddr(Opcodes.REGINDEX_SP), ConstAddr(symbols.size()))
        self._instructions.append(asm.Jump(op.label))
