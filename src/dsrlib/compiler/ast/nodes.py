#!/usr/bin/env python3

import contextlib

from dsrlib.compiler.mtypes import VOID, INT, FLOAT, FunctionType, MethodType, StateMethodType, StateType, Type


class Variable:
    def __init__(self, name, type_, **kwargs):
        self.name = name
        self.type_ = type_
        super().__init__(**kwargs)

    def type(self):
        return self.type_

    def isLValue(self):
        if self.type() in (FLOAT, INT):
            return self.name not in ('IMUX', 'IMUY', 'IMUZ', 'DELTA')
        return False

    def isRValue(self):
        return True

    def __str__(self):
        return self.name


class BuiltinVariable(Variable):
    pass


class Member(Variable):
    pass


class Callable:
    def __init__(self, rettype, name, args, **kwargs):
        self.rettype = rettype
        self.name = name
        self.args = args
        super().__init__(**kwargs)

    def isLValue(self):
        return False

    def isRValue(self):
        return self.rettype.name != 'void'


class Function(Callable):
    def type(self):
        return FunctionType(self.rettype, self.args)


class Method(Callable):
    def type(self):
        return MethodType(self.rettype, self.args)


class StateMethod(Callable):
    def type(self):
        return StateMethodType()


def ensureType(type_, expr):
    op = type_.cast(expr)

    if isinstance(expr, ConstantNode):
        return ConstantNode(value=type_.value(expr.value), pos=expr.position)

    if op is None:
        return expr
    return UnaryNode(op=op, expr=expr, pos=expr.position)


class Node:
    def __init__(self, *, pos):
        self.position = pos

    def type(self):
        raise NotImplementedError


class ScopedNodeMixin:
    def __init__(self, *args, symbols, **kwargs):
        self.symbols = symbols
        super().__init__(*args, **kwargs)


class ExpressionNode(Node):
    def type(self):
        raise NotImplementedError

    def isLValue(self):
        return False

    def isRValue(self):
        return True


class EmptyNode(ExpressionNode):
    def type(self):
        return VOID

    def isRValue(self):
        return False


class ConstantNode(ExpressionNode):
    def __init__(self, *, value, pos):
        self.value = value
        super().__init__(pos=pos)

    def type(self):
        if isinstance(self.value, float):
            return FLOAT
        if isinstance(self.value, int):
            return INT
        raise RuntimeError('Should not happen')


class IdentifierNode(ExpressionNode):
    def __init__(self, *, name, value, pos):
        self.name = name
        self.value = value
        super().__init__(pos=pos)

    def type(self):
        return self.value.type()

    def isLValue(self):
        return self.value.isLValue()

    def isRValue(self):
        return self.value.isRValue()


class PostfixUnaryNode(ExpressionNode):
    def __init__(self, *, target, op, pos):
        self.target = target
        self.op = op

        super().__init__(pos=pos)

    def type(self):
        return self.target.type()


class PrefixUnaryNode(ExpressionNode):
    def __init__(self, *, target, op, pos):
        self.target = target
        self.op = op

        super().__init__(pos=pos)

    def type(self):
        return self.target.type()


class UnaryNode(ExpressionNode):
    def __init__(self, *, expr, op, pos):
        self.expr = expr
        self.op = op
        super().__init__(pos=pos)

    def type(self):
        if self.op == 'casti':
            return INT
        if self.op == 'castf':
            return FLOAT
        return self.expr.type()


class BinaryNode(ExpressionNode):
    def __init__(self, *, op1, op, op2, pos):
        self.type_ = Type.maxType(op1.type(), op2.type())
        self.op1 = ensureType(self.type_, op1)
        self.op = op
        self.op2 = ensureType(self.type_, op2)

        super().__init__(pos=pos)

    def type(self):
        return self.type_


class CallNode(ExpressionNode):
    def __init__(self, *, target, args, pos):
        self.args = []
        for arg, ref in zip(args, target.type().args):
            arg = ensureType(ref.type(), arg)
            self.args.append(arg)

        self.target = target
        super().__init__(pos=pos)

    def type(self):
        return self.target.type().rettype

    def isLValue(self):
        return self.target.isLValue()

    def isRValue(self):
        return self.target.isRValue()


class AccessNode(ExpressionNode):
    def __init__(self, *, target, name, pos):
        self.target = target
        self.name = name
        super().__init__(pos=pos)

    def isLValue(self):
        type_ = self.target.type().memberType(self.name)
        return type_ in (INT, FLOAT)

    def isRValue(self):
        return self.target.isRValue()

    def type(self):
        return self.target.type().memberType(self.name)


class VariableNode(Variable, Node):
    def __init__(self, *, name, type_, expr, pos):
        if not isinstance(expr, EmptyNode):
            expr = ensureType(type_, expr)

        self.expr = expr
        super().__init__(name, type_, pos=pos)

    def type(self):
        return self.type_


class MemberNode(Member, Node):
    pass


class ArgumentNode(Variable, Node):
    def __init__(self, *, name, type_, pos):
        super().__init__(name, type_, pos=pos)


class StructNode(ScopedNodeMixin, Node):
    def __init__(self, *, struct, symbols, pos):
        self.struct = struct
        super().__init__(symbols=symbols, pos=pos)

    def type(self):
        return VOID


class StateNode(ScopedNodeMixin, Node):
    def __init__(self, *, name, init, main, symbols, pos):
        self.name = name
        self.init = init
        self.main = main
        super().__init__(symbols=symbols, pos=pos)

    def type(self):
        return StateType(self.init, self.main, self.symbols)


class IfNode(Node):
    def __init__(self, *, expr, yes, no, pos):
        self.expr = expr
        self.yes = yes
        self.no = no

        super().__init__(pos=pos)

    def type(self):
        return VOID

    def isRValue(self):
        return False


class WhileNode(Node):
    def __init__(self, *, expr, body, pos):
        self.expr = ensureType(INT, expr)
        self.body = body

        super().__init__(pos=pos)

    def type(self):
        return VOID

    def isRValue(self):
        return False


class AssignmentNode(Node):
    def __init__(self, *, target, op, expr, pos):
        self.target = target
        self.op = op
        self.expr = ensureType(self.target.type(), expr)

        super().__init__(pos=pos)

    def type(self):
        return VOID

    def isRValue(self):
        return False


class ContinueNode(Node):
    def type(self):
        return VOID

    def isRValue(self):
        return False


class BreakNode(Node):
    def type(self):
        return VOID

    def isRValue(self):
        return False


class YieldNode(Node):
    def type(self):
        return VOID

    def isRValue(self):
        return False


class GoNode(Node):
    def __init__(self, *, target, pos):
        self.target = target
        super().__init__(pos=pos)

    def type(self):
        return VOID

    def isRValue(self):
        return False


class ReturnNode(Node):
    def __init__(self, *, expr, pos):
        self.expr = expr
        self.func = None # Filled by ReturnChecker
        super().__init__(pos=pos)

    def type(self):
        return VOID

    def isRValue(self):
        return False

    def ensureType(self, type_):
        self.expr = ensureType(type_, self.expr)


class CallableNodeMixin(ScopedNodeMixin):
    def __init__(self, *, rettype, name, args, body, symbols, pos):
        self.body = body
        super().__init__(rettype, name, args, symbols=symbols, pos=pos)


class FunctionNode(CallableNodeMixin, Function, Node):
    pass


class MethodNode(CallableNodeMixin, Method, Node):
    pass


class StateMethodNode(CallableNodeMixin, StateMethod, Node):
    def __init__(self, *, name, body, symbols, pos):
        super().__init__(rettype=VOID, name=name, args=[], body=body, symbols=symbols, pos=pos)


class CompoundStatementNode(ScopedNodeMixin, Node):
    def __init__(self, *, statements, symbols, pos):
        self.statements = statements
        super().__init__(symbols=symbols, pos=pos)

    def type(self):
        return VOID

    def isRValue(self):
        return False


class ASTVisitor:
    def visit(self, node, **kwargs):
        return getattr(self, 'visit%s' % node.__class__.__name__)(node, **kwargs)

    def visitEmptyNode(self, node):
        raise NotImplementedError

    def visitConstantNode(self, node):
        raise NotImplementedError

    def visitIdentifierNode(self, node):
        raise NotImplementedError

    def visitPostfixUnaryNode(self, node):
        raise NotImplementedError

    def visitPrefixUnaryNode(self, node):
        raise NotImplementedError

    def visitUnaryNode(self, node):
        raise NotImplementedError

    def visitBinaryNode(self, node):
        raise NotImplementedError

    def visitCallNode(self, node):
        raise NotImplementedError

    def visitAccessNode(self, node):
        raise NotImplementedError

    def visitVariableNode(self, node):
        raise NotImplementedError

    def visitMemberNode(self, node):
        raise NotImplementedError

    def visitArgumentNode(self, node):
        raise NotImplementedError

    def visitStructNode(self, node):
        raise NotImplementedError

    def visitStateNode(self, node):
        raise NotImplementedError

    def visitIfNode(self, node):
        raise NotImplementedError

    def visitWhileNode(self, node):
        raise NotImplementedError

    def visitAssignmentNode(self, node):
        raise NotImplementedError

    def visitContinueNode(self, node):
        raise NotImplementedError

    def visitBreakNode(self, node):
        raise NotImplementedError

    def visitYieldNode(self, node):
        raise NotImplementedError

    def visitGoNode(self, node):
        raise NotImplementedError

    def visitReturnNode(self, node):
        raise NotImplementedError

    def visitFunctionNode(self, node):
        raise NotImplementedError

    def visitMethodNode(self, node):
        raise NotImplementedError

    def visitStateMethodNode(self, node):
        raise NotImplementedError

    def visitCompoundStatementNode(self, node):
        raise NotImplementedError


class ASTScopedVisitorMixin:
    def __init__(self, *args, **kwargs):
        self.__scopes = [] # pylint: disable=C0103
        super().__init__(*args, **kwargs)

    def visit(self, node):
        if isinstance(node, ScopedNodeMixin):
            with self.scope(node.symbols):
                return super().visit(node)
        return super().visit(node)

    @contextlib.contextmanager
    def scope(self, symbols):
        self.__scopes.append(symbols)
        try:
            yield
        finally:
            self.__scopes.pop()

    def currentTable(self):
        return self.__scopes[-1]

    def hasSymbol(self, name):
        return self.__scopes[-1].has(name)

    def getSymbol(self, name):
        return self.__scopes[-1].get(name)


class ASTLoopVisitorMixin:
    def __init__(self, *args, **kwargs):
        self.__stack = [] # pylint: disable=C0103
        super().__init__(*args, **kwargs)

    def visit(self, node):
        if isinstance(node, WhileNode):
            with self.loop(node):
                return super().visit(node)
        return super().visit(node)

    def createLoop(self, node):
        raise NotImplementedError

    def isInLoop(self):
        return len(self.__stack) != 0

    def currentLoop(self):
        return self.__stack[-1]

    @contextlib.contextmanager
    def loop(self, node):
        self.__stack.append(self.createLoop(node))
        try:
            yield
        finally:
            self.__stack.pop()


class ASTStateVisitorMixin:
    def __init__(self, *args, **kwargs):
        self.__stack = [] # pylint: disable=C0103
        self.__items = [] # pylint: disable=C0103
        super().__init__(*args, **kwargs)

    def visit(self, node):
        if isinstance(node, StateNode):
            with self.state(node):
                return super().visit(node)
        return super().visit(node)

    def createState(self, node):
        raise NotImplementedError

    def isInState(self):
        return len(self.__stack) != 0

    def currentState(self):
        return self.__stack[-1]

    def getState(self, node):
        for _node, item in self.__items:
            if node is _node:
                return item
        raise RuntimeError('no such state "%s"' % node.name)

    def getDefaultState(self):
        for node, item in self.__items:
            if node.name == 'idle':
                return item
        raise RuntimeError('no idle state')

    @contextlib.contextmanager
    def state(self, node):
        item = self.createState(node)
        self.__items.append((node, item))
        self.__stack.append(item)
        try:
            yield
        finally:
            self.__stack.pop()


class ASTCallableVisitorMixin:
    def __init__(self, *args, **kwargs):
        self.__items = [] # pylint: disable=C0103
        super().__init__(*args, **kwargs)

    def createCallable(self, node):
        raise NotImplementedError

    def visit(self, node):
        if isinstance(node, Callable):
            self.__items.append((node, self.createCallable(node)))
        return super().visit(node)

    def getCallable(self, node):
        for _node, item in self.__items:
            if node is _node:
                return item
        raise RuntimeError('no such callable "%s"' % node.name)
