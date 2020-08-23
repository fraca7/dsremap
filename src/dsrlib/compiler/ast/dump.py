#!/usr/bin/env python3

from .nodes import ASTVisitor


class Stringifier(ASTVisitor):
    def stringify(self, node):
        return self.visit(node)

    def visitIdentifierNode(self, node):
        return node.name

    def visitConstantNode(self, node):
        return str(node.value)

    def visitBinaryNode(self, node):
        return '(%s %s %s)' % (self.stringify(node.op1), node.op, self.stringify(node.op2))

    def visitUnaryNode(self, node):
        return '%s %s' % (node.op, self.stringify(node.expr))

    def visitPostfixUnaryNode(self, node):
        return '%s %s' % (self.stringify(node.target), node.op)

    def visitPrefixUnaryNode(self, node):
        return '%s %s' % (node.op, self.stringify(node.target))

    def visitAccessNode(self, node):
        return '%s.%s' % (self.stringify(node.target), node.name)

    def visitCallNode(self, node):
        return '%s(%s)' % (node.target.name, ', '.join([self.stringify(arg) for arg in node.args]))

    def visitEmptyNode(self, node): # pylint: disable=W0613
        return '(empty)'

    def visitArgumentNode(self, node):
        pass

    def visitAssignmentNode(self, node):
        pass

    def visitBreakNode(self, node):
        pass

    def visitCompoundStatementNode(self, node):
        pass

    def visitContinueNode(self, node):
        pass

    def visitFunctionNode(self, node):
        pass

    def visitIfNode(self, node):
        pass

    def visitMemberNode(self, node):
        pass

    def visitMethodNode(self, node):
        pass

    def visitStateMethodNode(self, node):
        pass

    def visitReturnNode(self, node):
        pass

    def visitStructNode(self, node):
        pass

    def visitStateNode(self, node):
        pass

    def visitVariableNode(self, node):
        pass

    def visitWhileNode(self, node):
        pass

    def visitYieldNode(self, node):
        pass

    def visitGoNode(self, node):
        pass


# pylint: disable=W0221

class Dumper(ASTVisitor):
    def __init__(self, stream):
        self.stream = stream
        self._stringifier = Stringifier()

    def stringify(self, node):
        return self._stringifier.stringify(node)

    def _display(self, text, indent):
        self.stream.write('%s%s\n' % ('  ' * indent, text))

    def dumpSymbolTable(self, symbols, indent):
        for symbol in symbols:
            self._display('Symbol: %s' % symbol.name, indent)

    def dump(self, node, indent=0):
        self.visit(node, indent=indent)

    def visitEmptyNode(self, node, indent): # pylint: disable=W0613
        self._display('(empty)', indent=indent)

    def visitPostfixUnaryNode(self, node, indent):
        self._display('Expression: %s' % self.stringify(node), indent)

    def visitPrefixUnaryNode(self, node, indent):
        self._display('Expression: %s' % self.stringify(node), indent)

    def visitConstantNode(self, node, indent):
        self._display('Const (%s): %s' % (node.type(), node.value), indent)

    def visitCompoundStatementNode(self, node, indent):
        self._display('Block', indent)
        self.dumpSymbolTable(node.symbols, indent=indent + 1)
        for statement in node.statements:
            self.dump(statement, indent=indent + 1)

    def visitStructNode(self, node, indent):
        self._display('Struct "%s"' % node.struct.name, indent)
        self.dumpSymbolTable(node.symbols, indent=indent + 1)
        for member in node.struct.methods:
            self.dump(member, indent=indent + 1)

    def visitStateNode(self, node, indent):
        self._display('State "%s"' % node.name, indent)
        self.dumpSymbolTable(node.symbols, indent=indent + 1)
        self.dump(node.init, indent=indent + 1)
        self.dump(node.main, indent=indent + 1)

    def visitMethodNode(self, node, indent):
        self._dumpCallableNode('Method', node, indent)

    def visitStateMethodNode(self, node, indent):
        self._dumpCallableNode('State method', node, indent)

    def visitFunctionNode(self, node, indent):
        self._dumpCallableNode('Function', node, indent)

    def _dumpCallableNode(self, label, node, indent):
        self._display('%s (%s): %s (%s)' % (label, node.rettype, node.name, ', '.join(['%s %s' % (arg.type_, arg.name) for arg in node.args])), indent)
        self.dumpSymbolTable(node.symbols, indent=indent)
        self.dump(node.body, indent=indent + 1)

    def visitAssignmentNode(self, node, indent):
        self._display('Assignment %s %s %s' % (self.stringify(node.target), node.op, self.stringify(node.expr)), indent)

    def visitVariableNode(self, node, indent):
        self._display('Declare %s (%s), initialize to %s' % (node.name, node.type(), self.stringify(node.expr)), indent)

    def visitIfNode(self, node, indent):
        self._display('Conditional on %s' % self.stringify(node.expr), indent)
        self._display('True branch:', indent)
        self.dump(node.yes, indent=indent + 1)
        self._display('False branch:', indent)
        self.dump(node.no, indent=indent + 1)

    def visitReturnNode(self, node, indent):
        self._display('Return %s' % self.stringify(node.expr), indent)

    def visitGoNode(self, node, indent):
        self._display('Go to state %s' % node.target, indent)

    def visitCallNode(self, node, indent):
        self._display('Call %s' % self.stringify(node), indent)

    def visitWhileNode(self, node, indent):
        self._display('While %s' % self.stringify(node.expr), indent)
        self.dump(node.body, indent=indent + 1)

    def visitAccessNode(self, node, indent):
        pass

    def visitArgumentNode(self, node, indent):
        pass

    def visitBinaryNode(self, node, indent):
        pass

    def visitBreakNode(self, node, indent):
        self._display('Break', indent)

    def visitContinueNode(self, node, indent):
        self._display('Continue', indent)

    def visitIdentifierNode(self, node, indent):
        pass

    def visitMemberNode(self, node, indent):
        pass

    def visitUnaryNode(self, node, indent):
        pass

    def visitYieldNode(self, node, indent):
        self._display('Yield', indent)
