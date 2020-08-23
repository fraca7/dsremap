#!/usr/bin/env python3

from .nodes import ASTVisitor, ConstantNode


class ConstEvaluator(ASTVisitor):
    def evaluate(self, node):
        return self.visit(node)

    def visitEmptyNode(self, node):
        return None

    def visitConstantNode(self, node):
        return node.value

    def visitIdentifierNode(self, node):
        return None

    def visitPostfixUnaryNode(self, node):
        return None

    def visitPrefixUnaryNode(self, node):
        return None

    def visitUnaryNode(self, node):
        val = self.evaluate(node.expr)
        if val is None:
            return None

        if node.op == '!':
            return 0 if val else 1
        if node.op == '-':
            return -val
        if node.op == '+':
            return val
        if node.op == 'casti':
            return int(val)
        if node.op == 'castf':
            return float(val)

        raise RuntimeError('Unknown operator "%s"' % node.op)

    def visitBinaryNode(self, node): # pylint: disable=R0911,R0912
        val1 = self.evaluate(node.op1)
        if val1 is not None:
            node.op1 = ConstantNode(value=val1, pos=node.op1.position)

        val2 = self.evaluate(node.op2)
        if val2 is not None:
            node.op2 = ConstantNode(value=val2, pos=node.op2.position)

        if val1 is None or val2 is None:
            return None

        if node.op == '*':
            return val1 * val2
        if node.op == '/':
            # Cast operations have already been inserted, so both are of the same type
            return val1 // val2 if isinstance(val1, int) else val1 / val2
        if node.op == '+':
            return val1 + val2
        if node.op == '-':
            return val1 - val2
        if node.op == '<':
            return int(val1 < val2)
        if node.op == '>':
            return int(val1 > val2)
        if node.op == '<=':
            return int(val1 <= val2)
        if node.op == '>=':
            return int(val1 >= val2)
        if node.op == '==':
            return int(val1 == val2)
        if node.op == '!=':
            return int(val1 != val2)
        if node.op == '&&':
            return 1 if val1 and val2 else 0
        if node.op == '||':
            return 1 if val1 or val2 else 0

        raise RuntimeError('Unknown operator "%s"' % node.op)

    def visitCallNode(self, node):
        return None

    def visitAccessNode(self, node):
        return None

    def visitVariableNode(self, node):
        val = self.evaluate(node.expr)
        if val is not None:
            node.expr = ConstantNode(value=val, pos=node.position)

    def visitMemberNode(self, node):
        return None

    def visitArgumentNode(self, node):
        return None

    def visitStructNode(self, node):
        for method in node.struct.methods:
            self.evaluate(method)

    def visitStateNode(self, node):
        self.evaluate(node.init)
        self.evaluate(node.main)

    def visitIfNode(self, node):
        val = self.evaluate(node.expr)
        if val is not None:
            node.expr = ConstantNode(value=val, pos=node.position)
        self.evaluate(node.yes)
        self.evaluate(node.no)

    def visitWhileNode(self, node):
        val = self.evaluate(node.expr)
        if val is not None:
            node.expr = ConstantNode(value=val, pos=node.position)
        self.evaluate(node.body)

    def visitAssignmentNode(self, node):
        val = self.evaluate(node.expr)
        if val is not None:
            node.expr = ConstantNode(value=val, pos=node.position)

    def visitContinueNode(self, node):
        return None

    def visitBreakNode(self, node):
        return None

    def visitYieldNode(self, node):
        return None

    def visitGoNode(self, node):
        return None

    def visitReturnNode(self, node):
        val = self.evaluate(node.expr)
        if val is not None:
            node.expr = ConstantNode(value=val, pos=node.position)

    def visitFunctionNode(self, node):
        self.evaluate(node.body)

    def visitMethodNode(self, node):
        self.evaluate(node.body)

    def visitStateMethodNode(self, node):
        self.evaluate(node.body)

    def visitCompoundStatementNode(self, node):
        for statement in node.statements:
            self.evaluate(statement)
