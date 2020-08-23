#!/usr/bin/env python3

from .nodes import ASTLoopVisitorMixin, ASTVisitor


class LoopChecker(ASTLoopVisitorMixin, ASTVisitor):
    def __init__(self, errors, warnings):
        self._errors = errors
        self._warnings = warnings
        super().__init__()

    def createLoop(self, node):
        return None

    def check(self, node):
        self.visit(node)

    def visitWhileNode(self, node):
        self.check(node.body)

    def visitStructNode(self, node):
        for method in node.struct.methods:
            self.check(method)

    def visitStateNode(self, node):
        self.check(node.init)
        self.check(node.main)

    def visitIfNode(self, node):
        self.check(node.yes)
        self.check(node.no)

    def visitContinueNode(self, node):
        if not self.isInLoop():
            self._errors.append(('continue statement out of loop', node.position))

    def visitBreakNode(self, node):
        if not self.isInLoop():
            self._errors.append(('break statement out of loop', node.position))

    def visitFunctionNode(self, node):
        self.check(node.body)

    def visitMethodNode(self, node):
        self.check(node.body)

    def visitStateMethodNode(self, node):
        self.check(node.body)

    def visitCompoundStatementNode(self, node):
        for statement in node.statements:
            self.check(statement)

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

    def visitMemberNode(self, node):
        pass

    def visitArgumentNode(self, node):
        pass

    def visitAssignmentNode(self, node):
        pass

    def visitYieldNode(self, node):
        pass

    def visitReturnNode(self, node):
        pass

    def visitGoNode(self, node):
        pass
