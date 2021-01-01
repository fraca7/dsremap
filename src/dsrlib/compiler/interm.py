#!/usr/bin/env python3

import contextlib
import collections

from .mtypes import MethodType, VOID, FLOAT, INT
from .ast.nodes import Variable, EmptyNode, StateNode, VariableNode, IdentifierNode, Callable, \
     ASTScopedVisitorMixin, ASTLoopVisitorMixin, ASTStateVisitorMixin, ASTCallableVisitorMixin, ASTVisitor, \
     ArgumentNode


class Address:
    def type(self):
        raise NotImplementedError


class Counter:
    def __init__(self):
        self._count = 0

    def next(self):
        try:
            return self._count
        finally:
            self._count += 1


class TempVar(Address):
    def __init__(self, type_, symbols, counter):
        self._id = counter.next()
        self._type = type_
        self.name = self
        symbols.add(self, self)

    def __str__(self):
        return 'var_%d' % self._id

    def type(self):
        return self._type


class Const(Address):
    def __init__(self, value):
        self.value = value

    def type(self):
        return INT if isinstance(self.value, int) else FLOAT

    def __str__(self):
        return str(self.value)

    def __eq__(self, other):
        return isinstance(other, Const) and type(self.value) is type(other.value) and self.value == other.value


class Retval(Address):
    def __init__(self, function):
        self.function = function

    def type(self):
        return self.function.rettype

    def __str__(self):
        return 'retval'


class Member(Address):
    def __init__(self, node, symbols):
        self._type = node.target.type().memberType(node.name)

        fields = []
        while not isinstance(node, IdentifierNode):
            fields.append(node.name)
            node = node.target

        self.root = symbols.get(node.name).value
        self.fields = list(reversed(fields))

    def type(self):
        return self._type

    def __str__(self):
        return '%s.%s' % (self.root.name, '.'.join(self.fields))


class Label:
    def __init__(self, counter):
        self._id = counter.next()

    def __str__(self):
        return 'label_%d' % self._id


class CallableLabel(Label):
    def __init__(self, func, counter):
        super().__init__(counter)
        self.func = func


class StateEnterLabel(Label):
    def __init__(self, state, counter):
        super().__init__(counter)
        self.state = state


class CompoundStartLabel(Label):
    def __init__(self, symbols, counter):
        super().__init__(counter)
        self.symbols = symbols


class CompoundEndLabel(Label):
    def __init__(self, symbols, counter):
        super().__init__(counter)
        self.symbols = symbols


class LineSpec:
    def __init__(self, line):
        self.line = line

    def __str__(self):
        return '# %d' % self.line


class UnaryOp:
    def __init__(self, symbols, dst, op, op1):
        self.symbols = symbols
        self.dst = dst
        self.op = op
        self.op1 = op1

        assert isinstance(dst, (Address, Variable))
        assert isinstance(op1, (Address, Variable))

    def __str__(self):
        return '\t%s <- %s %s' % (self.dst, self.op, self.op1)


class BinaryOp:
    def __init__(self, symbols, dst, op, op1, op2):
        self.symbols = symbols
        self.dst = dst
        self.op = op
        self.op1 = op1
        self.op2 = op2

        assert isinstance(dst, (Address, Variable))
        assert isinstance(op1, (Address, Variable))
        assert isinstance(op2, (Address, Variable))

    def __str__(self):
        return '\t%s <- %s %s %s' % (self.dst, self.op1, self.op, self.op2)


class Assignment:
    def __init__(self, symbols, dst, src):
        self.symbols = symbols
        self.dst = dst
        self.src = src

        assert isinstance(dst, (Address, Variable))
        assert isinstance(src, (Address, Variable))

    def __str__(self):
        return '\t%s <- %s' % (self.dst, self.src)


class Return:
    def __init__(self, symbols, func, var=None):
        self.symbols = symbols
        self.func = func
        self.var = var

        assert isinstance(var, (Address, ArgumentNode, type(None)))

    def __str__(self):
        return '\tret' if self.var is None else '\tret %s' % str(self.var)


class IfFalse:
    def __init__(self, symbols, cond, label):
        self.symbols = symbols
        self.cond = cond
        self.label = label

        assert isinstance(cond, (Address, Variable))

    def __str__(self):
        return '\tifz %s %s' % (self.cond, self.label)


class Jump:
    def __init__(self, label):
        self.label = label

    def __str__(self):
        return '\tjmp %s' % self.label


class Argument:
    def __init__(self, symbols, var):
        self.symbols = symbols
        self.var = var

        assert isinstance(var, (Address, Variable))

    def __str__(self):
        return '\targ %s' % str(self.var)


class MethodCall:
    def __init__(self, symbols, var, method):
        self.symbols = symbols
        self.var = var
        self.method = method
        self.label = None

        assert isinstance(var, (Address, Variable))

    def __str__(self):
        return '\tmcall %d, %s.%s' % (len(self.method.type().args), self.var, self.method.name)


class FunctionCall:
    def __init__(self, function):
        self.function = function
        self.label = None

    def __str__(self):
        return '\tfcall %s' % self.label


class Yield:
    def __str__(self):
        return '\tyield'


class Go:
    def __init__(self, symbols, state):
        self.symbols = symbols
        self.state = state
        self.label = None

    def __str__(self):
        return '\tgo\t%s' % self.state.name


class ExpressionCache:
    def __init__(self, symbols):
        self._values = []
        self._symbols = symbols

    def var(self, op):
        for var, _op in self._values:
            if _op == op:
                return var
        return None

    def add(self, var, op):
        self._values.append((var, op))
        return var


class ExpressionCacheStack:
    def __init__(self):
        self._values = []

    def var(self, op):
        for cache in reversed(self._values):
            var = cache.var(op)
            if var is not None:
                return var
        return None

    def add(self, var, op):
        self._values[-1].add(var, op)

    def push(self, symbols):
        self._values.append(ExpressionCache(symbols))

    def pop(self):
        self._values.pop()


LoopLabels = collections.namedtuple('LoopLabel', ['start', 'end'])
StateLabels = collections.namedtuple('StateLabels', ['init', 'main'])


class ICGenerator(ASTScopedVisitorMixin, ASTLoopVisitorMixin, ASTStateVisitorMixin, ASTCallableVisitorMixin, ASTVisitor):
    def __init__(self):
        self._expressions = ExpressionCacheStack()
        self._blocks = [[]]
        self._line = 0
        self._unresolved = []
        self._varcounter = Counter()
        self._labelcounter = Counter()

        super().__init__()

    def addOp(self, op):
        self._blocks[-1].append(op)

    def createLoop(self, node):
        return LoopLabels(Label(self._labelcounter), Label(self._labelcounter))

    def createState(self, node):
        return StateLabels(StateEnterLabel(node, self._labelcounter), Label(self._labelcounter))

    def createCallable(self, node):
        return CallableLabel(node, self._labelcounter)

    @contextlib.contextmanager
    def scope(self, symbols):
        self._expressions.push(symbols)
        self._blocks.append([])
        try:
            with super().scope(symbols) as ret:
                yield ret
        finally:
            self._expressions.pop()
            ops = self._blocks.pop()
            self._blocks[-1].extend(ops)

    def var(self, op):
        return self._expressions.var(op)

    def add(self, var, op):
        self._expressions.add(var, op)

    def generate(self, node):
        # Assuming the node is the top-level compound statement. Reorder everything to pu
        # the global variables first, and the idle state next.

        def key(node):
            if isinstance(node, VariableNode):
                return 0
            if isinstance(node, StateNode):
                return 1 if node.name == 'idle' else 2
            return 3
        node.statements.sort(key=key)

        self.visit(node)

        for _node, func in self._unresolved:
            if isinstance(_node, StateNode):
                label, _ = self.getState(_node)
            elif isinstance(_node, Callable):
                label = self.getCallable(_node)
            else:
                raise RuntimeError(_node)
            func(label)

        return self._blocks[0]

    def encode(self, node):
        if node.position is not None and node.position.line != self._line:
            self._line = node.position.line
            self.addOp(LineSpec(self._line))
        return self.visit(node)

    def visitCompoundStatementNode(self, node):
        self.addOp(CompoundStartLabel(node.symbols, self._labelcounter))
        for statement in node.statements:
            self.encode(statement)
        self.addOp(CompoundEndLabel(node.symbols, self._labelcounter))

    def visitVariableNode(self, node):
        if node.expr.isRValue():
            src = self.encode(node.expr)
            self.addOp(Assignment(self.currentTable(), node, src))
        return node

    def visitAssignmentNode(self, node):
        var = self.encode(node.target)
        val = self.encode(node.expr)
        if node.op == '=':
            self.addOp(Assignment(self.currentTable(), var, val))
        else:
            op = {'+=': '+', '-=': '-', '*=': '*', '/=': '/'}[node.op]
            self.addOp(BinaryOp(self.currentTable(), var, op, var, val))

    def visitIdentifierNode(self, node):
        return node.value

    def visitConstantNode(self, node):
        return Const(node.value)

    def visitUnaryNode(self, node):
        op1 = self.encode(node.expr)
        var = self.var((node.op, op1, None))
        if var is None:
            var = TempVar(node.type(), self.currentTable(), self._varcounter)
            op = UnaryOp(self.currentTable(), var, node.op, op1)
            self.addOp(op)
            self.add(var, (node.op, op1, None))
        return var

    def visitBinaryNode(self, node):
        op1 = self.encode(node.op1)
        op2 = self.encode(node.op2)
        var = self.var((node.op, op1, op2))
        if var is None:
            var = TempVar(node.type(), self.currentTable(), self._varcounter)
            op = BinaryOp(self.currentTable(), var, node.op, op1, op2)
            self.addOp(op)
            self.add(var, (node.op, op1, op2))
        return var

    def visitStructNode(self, node):
        for method in node.struct.methods:
            self.encode(method)

    def visitStateNode(self, node):
        init, main = self.currentState()
        self.addOp(init)
        self.encode(node.init)
        self.addOp(main)
        self.encode(node.main)

    def encodeCallableNode(self, node):
        label = self.getCallable(node)
        self.addOp(label)
        self.encode(node.body)

    def visitFunctionNode(self, node):
        self.encodeCallableNode(node)

    def visitMethodNode(self, node):
        self.encodeCallableNode(node)

    def visitStateMethodNode(self, node):
        self.encode(node.body)
        if node.name != 'enter':
            _, main = self.currentState()
            self.addOp(Yield())
            self.addOp(Jump(main))

    def visitReturnNode(self, node):
        if node.expr.isRValue():
            var = self.encode(node.expr)
            self.addOp(Return(self.currentTable(), node.func, var))
        else:
            self.addOp(Return(self.currentTable(), node.func))

    def visitGoNode(self, node):
        state = self.getSymbol(node.target).value
        op = Go(self.currentTable(), state)
        self.addOp(op)
        def resolve(label):
            op.label = label
        self._unresolved.append((state, resolve))

    def visitIfNode(self, node):
        cond = self.encode(node.expr)
        l1 = Label(self._labelcounter) # true
        if isinstance(node.no, EmptyNode):
            self.addOp(IfFalse(self.currentTable(), cond, l1))
            self.encode(node.yes)
            self.addOp(l1)
        else:
            l2 = Label(self._labelcounter) # next
            self.addOp(IfFalse(self.currentTable(), cond, l1))
            self.encode(node.yes)
            self.addOp(Jump(l2))
            self.addOp(l1)
            self.encode(node.no)
            self.addOp(l2)

    def visitCallNode(self, node):
        for arg in node.args:
            var = self.encode(arg)
            self.addOp(Argument(self.currentTable(), var))

        ftype = node.target.type()
        retval = None if ftype.rettype is VOID else Retval(ftype)

        if isinstance(ftype, MethodType):
            symbols = node.target.target.type().symbols
            var = self.encode(node.target.target)
            function = symbols.get(node.target.name).value
            call = MethodCall(self.currentTable(), var, function)
        else:
            function = self.getSymbol(node.target.name).value
            call = FunctionCall(function)

        self.addOp(call)

        def resolve(label):
            call.label = label
        self._unresolved.append((function, resolve))

        if retval is not None:
            var = TempVar(retval.type(), self.currentTable(), self._varcounter)
            self.addOp(Assignment(self.currentTable(), var, retval))
            return var
        return None

    def visitAccessNode(self, node):
        return Member(node, self.currentTable())

    def visitWhileNode(self, node):
        l1, l2 = self.currentLoop()
        self.addOp(l1)
        var = self.encode(node.expr)
        self.addOp(IfFalse(self.currentTable(), var, l2))
        self.encode(node.body)
        self.addOp(Jump(l1))
        self.addOp(l2)

    def visitBreakNode(self, node):
        _, label = self.currentLoop()
        self.addOp(Jump(label))

    def visitContinueNode(self, node):
        label, _ = self.currentLoop()
        self.addOp(Jump(label))

    def visitYieldNode(self, node):
        self.addOp(Yield())

    def visitPrefixUnaryNode(self, node):
        op = {'++': '+', '--': '-'}[node.op]
        var = self.encode(node.target)
        self.addOp(BinaryOp(self.currentTable(), var, op, var, Const(1)))
        return var

    def visitPostfixUnaryNode(self, node):
        op = {'++': '+', '--': '-'}[node.op]
        var = self.encode(node.target)
        res = TempVar(node.type(), self.currentTable(), self._varcounter)
        self.addOp(Assignment(self.currentTable(), res, var))
        self.addOp(BinaryOp(self.currentTable(), var, op, var, Const(1)))
        return res

    def visitEmptyNode(self, node):
        pass

    def visitArgumentNode(self, node):
        pass

    def visitMemberNode(self, node):
        return node
