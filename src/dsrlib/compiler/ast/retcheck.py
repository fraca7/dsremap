#!/usr/bin/env python3

import contextlib

from dsrlib.compiler.mtypes import VOID, CastError

from .nodes import ASTVisitor, ASTScopedVisitorMixin, ASTStateVisitorMixin, ReturnNode, EmptyNode, StateNode


class ReturnChecker(ASTScopedVisitorMixin, ASTStateVisitorMixin, ASTVisitor):
    def __init__(self, errors, warnings):
        self._errors = errors
        self._warnings = warnings
        self._stack = [[None, 0, False]]
        super().__init__()

    def createState(self, node):
        return None

    @contextlib.contextmanager
    def enter(self, func):
        self._stack.append([func, 0, False])
        try:
            yield
        finally:
            _, _, returned = self._stack.pop()
            if not returned:
                if func.rettype == VOID:
                    node = ReturnNode(pos=None, expr=EmptyNode(pos=None))
                    node.func = func
                    func.body.statements.append(node)
                else:
                    self._errors.append(('no return statement in function', func.position))

    @contextlib.contextmanager
    def branch(self):
        self._stack[-1][1] += 1
        try:
            yield
        finally:
            self._stack[-1][1] -= 1

    def inFunc(self):
        return self._stack[-1][0] is not None

    def func(self):
        return self._stack[-1][0]

    def level(self):
        return self._stack[-1][1]

    def setReturned(self):
        self._stack[-1][2] = True

    def check(self, node):
        self.visit(node)

    def visitIfNode(self, node):
        with self.branch():
            self.visit(node.yes)
            self.visit(node.no)

    def visitWhileNode(self, node):
        with self.branch():
            self.visit(node.body)

    def visitReturnNode(self, node):
        if not self.inFunc():
            self._errors.append(('return statement outside of function/method', node.position))
            return

        node.func = self.func()

        try:
            node.ensureType(self.func().rettype)
        except CastError as exc:
            self._errors.append((str(exc), node.position))

        if self.level() == 0:
            self.setReturned()

    def visitGoNode(self, node):
        if not self.isInState():
            self._errors.append(('go statement outside of state method', node.position))
            return

        if not self.hasSymbol(node.target):
            self._errors.append(('unknown state name "%s"' % node.target, node.position))
            return

        state = self.getSymbol(node.target).value
        if not isinstance(state, StateNode):
            self._errors.append(('go statement target is not a state (%s)' % state.type(), node.position))

    def visitCompoundStatementNode(self, node):
        for statement in node.statements:
            self.visit(statement)

    def visitStructNode(self, node):
        for method in node.struct.methods:
            with self.enter(method):
                self.check(method.body)

    def visitStateNode(self, node):
        # Those are NOT functions
        self.check(node.init)
        self.check(node.main)

    def visitMethodNode(self, node):
        with self.enter(node):
            self.check(node.body)

    def visitFunctionNode(self, node):
        with self.enter(node):
            self.check(node.body)

    def visitStateMethodNode(self, node):
        self.check(node.body)

    def visitEmptyNode(self, node):
        pass

    def visitConstantNode(self, node):
        pass

    def visitIdentifierNode(self, node):
        pass

    def visitPostfixUnaryNode(self, node):
        pass

    def visitPrefixUnaryNode(self, node):
        pass

    def visitUnaryNode(self, node):
        pass

    def visitBinaryNode(self, node):
        pass

    def visitCallNode(self, node):
        pass

    def visitAccessNode(self, node):
        pass

    def visitVariableNode(self, node):
        pass

    def visitArgumentNode(self, node):
        pass

    def visitMemberNode(self, node):
        pass

    def visitAssignmentNode(self, node):
        pass

    def visitContinueNode(self, node):
        pass

    def visitBreakNode(self, node):
        pass

    def visitYieldNode(self, node):
        pass
