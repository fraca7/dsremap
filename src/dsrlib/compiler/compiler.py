#!/usr/bin/env python3

# pylint: disable=invalid-name, attribute-defined-outside-init, wildcard-import, unused-wildcard-import

import io

from ptk.parser import production, LRParser
from ptk.lexer import token, ReLexer

from .preprocessor import Preprocessor
from .ast import ConstEvaluator, ReturnChecker, LoopChecker
from .symbols import SymbolTable, SymbolScope

from .mtypes import *
from .ast.nodes import *


class FatalError(Exception):
    def __init__(self, msg, position):
        super().__init__(msg)
        self.position = position


class SymbolTableStack:
    def __init__(self):
        self._stack = [SymbolTable()] # Builtins and globals

    def has(self, name, recurse=True):
        return self._stack[-1].has(name, recurse=recurse)

    def add(self, name, value, size=None):
        self._stack[-1].add(name, value, size=size)

    def get(self, name):
        return self._stack[-1].get(name)

    def push(self):
        self._stack.append(SymbolTable(parent=self._stack[-1]))

    def pop(self):
        return self._stack.pop()


class Parser(LRParser, ReLexer):
    def parse(self, string):
        self._symbols = SymbolTableStack()

        # Builtins
        for type_ in (VOID, INT, FLOAT):
            self._symbols.add(type_.name, type_)
        for name in ('LPadX', 'LPadY', 'RPadX', 'RPadY', 'Hat', 'Square', 'Cross', 'Circle', 'Triangle', 'L1', 'R1', 'L2', 'R2',
                     'Share', 'Options', 'L3', 'R3', 'PS', 'TPad', 'L2Value', 'R2Value', 'IMUX', 'IMUY', 'IMUZ', 'DELTA'):
            self._addBuiltinVar(name)

        self._errors = []
        self._warnings = []
        self._ast = None

        try:
            super().parse(string)
        except FatalError as exc:
            self._errors.append((str(exc), exc.position))

        return self._ast, self._warnings, self._errors

    def _addBuiltinVar(self, name):
        type_ = {
            'IMUX': FLOAT,
            'IMUY': FLOAT,
            }.get(name, INT)
        self._symbols.add(name, BuiltinVariable(name, type_), size=0)

    def newSentence(self, sentence):
        ceval = ConstEvaluator()
        ceval.evaluate(sentence)

        checker = ReturnChecker(self._errors, self._warnings)
        checker.check(sentence)

        checker = LoopChecker(self._errors, self._warnings)
        checker.check(sentence)

        self._ast = sentence

    # Lexer

    def ignore(self, char):
        return char in (' ', '\t', '\n')

    @token(r'\+\+')
    def INC_OP(self, tok):
        pass

    @token(r'--')
    def DEC_OP(self, tok):
        pass

    @token(r'<=')
    def LE_OP(self, tok):
        pass

    @token(r'>=')
    def GE_OP(self, tok):
        pass

    @token(r'==')
    def EQ_OP(self, tok):
        pass

    @token(r'!=')
    def NE_OP(self, tok):
        pass

    @token(r'&&')
    def AND_OP(self, tok):
        pass

    @token(r'\|\|')
    def OR_OP(self, tok):
        pass

    @token(r'=')
    def ASSIGN(self, tok):
        pass

    @token(r'\*=')
    def MUL_ASSIGN(self, tok):
        pass

    @token(r'/=')
    def DIV_ASSIGN(self, tok):
        pass

    @token(r'\+=')
    def ADD_ASSIGN(self, tok):
        pass

    @token(r'-=')
    def SUB_ASSIGN(self, tok):
        pass

    @token(r'int')
    def INT(self, tok):
        pass

    @token(r'float')
    def FLOAT(self, tok):
        pass

    @token('void')
    def VOID(self, tok):
        pass

    @token(r'struct')
    def STRUCT(self, tok):
        pass

    @token(r'state')
    def STATE(self, tok):
        pass

    @token('yield')
    def YIELD(self, tok):
        pass

    @token(r'if')
    def IF(self, tok):
        pass

    @token(r'else')
    def ELSE(self, tok):
        pass

    @token(r'while')
    def WHILE(self, tok):
        pass

    @token(r'return')
    def RETURN(self, tok):
        pass

    @token(r'go')
    def GO(self, tok):
        pass

    @token(r'break')
    def BREAK(self, tok):
        pass

    @token('continue')
    def CONTINUE(self, tok):
        pass

    # Note: we don't allow unary - or + here, it's handled by the grammar

    @token(r'[0-9]*\.[0-9]+([eE][-+]?[0-9]+)?')
    def FLOAT_LIT(self, tok):
        tok.value = float(tok.value)

    @token(r'0x[0-9a-fA-F]+')
    @token(r'(0|([1-9][0-9]*))')
    def INTEGER_LIT(self, tok):
        tok.value = int(tok.value[2:], 16) if tok.value.startswith('0x') else int(tok.value)

    @token('[a-zA-Z_][a-zA-Z0-9_]*', types=['IDENTIFIER', 'TYPE_NAME'])
    def IDENTIFIER_OR_TYPE(self, tok):
        tok.type = 'TYPE_NAME' if self._symbols.has(tok.value) and isinstance(self._symbols.get(tok.value).value, Type) else 'IDENTIFIER'

    # Parser

    @production('program<pos> -> program_unit*<statements>')
    def program(self, pos, statements):
        if not self._symbols.has('idle'):
            self._errors.append(('missing idle state', pos))
        elif not isinstance(self._symbols.get('idle').value, StateNode):
            self._errors.append(('global "idle" symbol is not a state', pos))
        return CompoundStatementNode(statements=statements, symbols=self._symbols.pop(), pos=pos)

    @production('program_unit -> var_declaration<unit>')
    @production('program_unit -> function_definition<unit>')
    @production('program_unit -> struct_specifier<unit> ";"')
    @production('program_unit -> state_specifier<unit> ";"')
    def program_unit(self, unit):
        return unit

    # Expressions

    @production('primary_expression<pos> -> FLOAT_LIT<value>')
    @production('primary_expression<pos> -> INTEGER_LIT<value>')
    def primary_expression_constant(self, pos, value):
        return ConstantNode(value=value, pos=pos)

    @production('primary_expression -> "(" expression<expr> ")"')
    def primary_expression_expr(self, expr):
        return expr

    @production('primary_expression<pos> -> IDENTIFIER<name>')
    def primary_expression_identifier(self, pos, name):
        if self._symbols.has(name):
            return IdentifierNode(name=name, value=self._symbols.get(name).value, pos=pos)
        raise FatalError('undeclared identifier "%s"' % name, pos)

    @production('postfix_expression -> primary_expression<expr>')
    def postfix_expression_primary(self, expr):
        return expr

    @production('postfix_expression<pos> -> postfix_expression<target> "." IDENTIFIER<name>')
    def postfix_expression_access(self, pos, target, name):
        ttype = target.type()
        if not isinstance(ttype, StructType) or not ttype.hasMember(name):
            self._errors.append(('Variable of type "%s" has no "%s" member' % (ttype, name), pos))
            return ConstantNode(value=0, pos=pos)
        return AccessNode(target=target, name=name, pos=pos)

    @production('postfix_expression<pos> -> postfix_expression<target> INC_OP<op>')
    @production('postfix_expression<pos> -> postfix_expression<target> DEC_OP<op>')
    def postfix_expression_unary(self, pos, target, op):
        if not target.isLValue():
            self._errors.append(('postfix increment/decrement target is not an lvalue', pos))
        return PostfixUnaryNode(target=target, op=op, pos=pos)

    @production('call_expression<pos> -> postfix_expression<target> "(" expression*(",")<args> ")"')
    def call_expression(self, pos, target, args):
        ftype = target.type()
        if not isinstance(ftype, CallableType):
            raise FatalError('expression of type "%s" is not callable' % ftype, pos)
        if len(args) != len(ftype.args):
            raise FatalError('argument count mismatch', pos)

        try:
            return CallNode(target=target, args=args, pos=pos)
        except CastError as exc:
            raise FatalError(str(exc), pos)

    @production('unary_expression -> call_expression<expr>')
    @production('unary_expression -> postfix_expression<expr>')
    def unary_expression_call_postfix(self, expr):
        return expr

    @production('unary_expression<pos> -> INC_OP<op> postfix_expression<target>')
    @production('unary_expression<pos> -> DEC_OP<op> postfix_expression<target>')
    def unary_expression_incdec(self, pos, target, op):
        if not target.isLValue():
            self._errors.append(('prefix increment/decrement target is not an lvalue', pos))
        return PrefixUnaryNode(target=target, op=op, pos=pos)

    @production('unary_expression<pos> -> "+"<op> unary_expression<expr>')
    @production('unary_expression<pos> -> "-"<op> unary_expression<expr>')
    @production('unary_expression<pos> -> "!"<op> unary_expression<expr>')
    def unary_expression_unary_arith(self, pos, op, expr):
        if not expr.isRValue():
            self._errors.append(('"%s" unary operator target is not an rvalue' % op, pos))
            return expr

        try:
            return UnaryNode(expr=expr, op=op, pos=pos)
        except CastError as exc:
            self._errors.append((str(exc), pos))
            return expr

    @production('multiplicative_expression -> unary_expression<expr>')
    def multiplicative_expression_unary(self, expr):
        return expr

    @production('multiplicative_expression<pos> -> multiplicative_expression<op1> "*"<op> unary_expression<op2>')
    @production('multiplicative_expression<pos> -> multiplicative_expression<op1> "/"<op> unary_expression<op2>')
    @production('additive_expression<pos> -> additive_expression<op1> "+"<op> multiplicative_expression<op2>')
    @production('additive_expression<pos> -> additive_expression<op1> "-"<op> multiplicative_expression<op2>')
    @production('relational_expression<pos> -> relational_expression<op1> "<"<op> additive_expression<op2>')
    @production('relational_expression<pos> -> relational_expression<op1> ">"<op> additive_expression<op2>')
    @production('relational_expression<pos> -> relational_expression<op1> LE_OP<op> additive_expression<op2>')
    @production('relational_expression<pos> -> relational_expression<op1> GE_OP<op> additive_expression<op2>')
    @production('equality_expression<pos> -> equality_expression<op1> EQ_OP<op> relational_expression<op2>')
    @production('equality_expression<pos> -> equality_expression<op1> NE_OP<op> relational_expression<op2>')
    @production('logical_and_expression<pos> -> logical_and_expression<op1> AND_OP<op> equality_expression<op2>')
    @production('expression<pos> -> expression<op1> OR_OP<op> logical_and_expression<op2>')
    def binary_expression(self, pos, op1, op, op2):
        if not op1.isRValue():
            raise FatalError('left operand of "%s" is not an rvalue' % op, pos)
        if not op2.isRValue():
            raise FatalError('right operand of "%s" is not an rvalue' % op, pos)

        try:
            return BinaryNode(op1=op1, op=op, op2=op2, pos=pos)
        except CastError as exc:
            self._errors.append((str(exc), pos))
            return op1

    @production('additive_expression -> multiplicative_expression<expr>')
    def additive_expression_multiplicative(self, expr):
        return expr

    @production('relational_expression -> additive_expression<expr>')
    def relational_expression_additive(self, expr):
        return expr

    @production('equality_expression -> relational_expression<expr>')
    def equality_expression_relational(self, expr):
        return expr

    @production('logical_and_expression -> equality_expression<expr>')
    def logical_and_expression_equality(self, expr):
        return expr

    @production('expression -> logical_and_expression<expr>')
    def expression_logical_and(self, expr):
        return expr

    @production('assignment_operator -> ASSIGN<op>')
    @production('assignment_operator -> MUL_ASSIGN<op>')
    @production('assignment_operator -> DIV_ASSIGN<op>')
    @production('assignment_operator -> ADD_ASSIGN<op>')
    @production('assignment_operator -> SUB_ASSIGN<op>')
    def assignment_operator(self, op):
        return op

    @production('var_declaration<pos> -> type_specifier<type_> init_declarator<var> ";"')
    def var_declaration(self, pos, type_, var):
        name, expr = var

        if self._symbols.has(name, recurse=False):
            self._errors.append(('"%s" redefined' % name, pos))
        elif self._symbols.has(name):
            self._warnings.append(('"%s" shadows variable in outer scope' % name, pos))

        if type_.name == 'void':
            self._errors.append(('cannot declare variable of type "void"', pos))
            type_ = INT

        try:
            node = VariableNode(name=name, type_=type_, expr=expr, pos=pos)
        except CastError as exc:
            raise FatalError(str(exc), pos)

        self._symbols.add(name, node)
        return node

    @production('member_declaration<pos> -> type_specifier<type_> IDENTIFIER<name> ";"')
    def member_declaration(self, pos, type_, name):
        if self._symbols.has(name, recurse=False):
            self._errors.append(('"%s" redefined' % name, pos))
        elif self._symbols.has(name):
            self._warnings.append(('"%s" shadows variable in outer scope' % name, pos))

        if type_.name == 'void':
            self._errors.append(('cannot declare member of type "void"', pos))
            type_ = INT

        node = MemberNode(name=name, type_=type_, pos=pos)
        self._symbols.add(name, node)
        return node

    @production('init_declarator<pos> -> IDENTIFIER<name>')
    @production('init_declarator<pos> -> IDENTIFIER<name> ASSIGN expression<expr>')
    def init_declarator(self, pos, name, expr=None):
        return (name, expr or EmptyNode(pos=pos))

    @production('type_specifier -> VOID<name>')
    @production('type_specifier -> INT<name>')
    @production('type_specifier -> FLOAT<name>')
    @production('type_specifier -> TYPE_NAME<name>')
    def type_specifier(self, name):
        return self._symbols.get(name).value

    @production('block_start -> "{"')
    def block_start(self):
        self._symbols.push()

    @production('block_end -> "}"')
    def block_end(self):
        return self._symbols.pop()

    @production('struct_specifier<pos> -> STRUCT IDENTIFIER<name> block_start struct_declaration+<decls> block_end<symbols>')
    def struct_specifier(self, pos, name, decls, symbols):
        if self._symbols.has(name, recurse=False):
            self._warnings.append(('struct name "%s" shadows symbol in outer scope' % name, pos))
        elif self._symbols.has(name):
            self._errors.append(('duplicate identifier "%s"' % name, pos))

        members = [member for member in decls if isinstance(member, Member)]
        methods = [method for method in decls if isinstance(method, Method)]

        type_ = StructType(name, members, methods, symbols)
        self._symbols.add(name, type_)
        symbols.setType(SymbolScope.THIS)

        return StructNode(struct=type_, symbols=symbols, pos=pos)

    @production('struct_declaration -> member_declaration<decl>')
    @production('struct_declaration -> method_definition<decl>')
    def struct_declaration_decl(self, decl):
        return decl

    @production('state_specifier<pos> -> STATE IDENTIFIER<name> block_start state_declaration+<decls> block_end<symbols>')
    def state_specifier(self, pos, name, decls, symbols):
        if self._symbols.has(name, recurse=False):
            self._warnings.append(('state name "%s" shadows symbol in outer scope' % name, pos))
        elif self._symbols.has(name):
            self._errors.append(('duplicate identifier "%s"' % name, pos))

        seen = set()
        init = EmptyNode(pos=pos)
        main = EmptyNode(pos=pos)
        for method in decls:
            if not isinstance(method, StateMethodNode):
                continue
            if method.name in seen:
                self._errors.append(('duplicate state method name "%s" in state "%s"' % (method.name, name), pos))
                continue
            seen.add(method.name)
            if method.name == 'enter':
                init = method
            elif method.name == name:
                main = method
            else:
                self._errors.append(('invalid state method name "%s" in state "%s"' % (method.name, name), pos))

        node = StateNode(name=name, init=init, main=main, symbols=symbols, pos=pos)
        self._symbols.add(name, node)
        symbols.setType(SymbolScope.INHERITED)
        return node

    @production('state_declaration -> member_declaration<decl>')
    @production('state_declaration -> state_method_definition<decl>')
    def state_declaration(self, decl):
        return decl

    @production('statement -> compound_statement<st>')
    @production('statement -> expression_statement<st>')
    @production('statement -> selection_statement<st>')
    @production('statement -> iteration_statement<st>')
    @production('statement -> jump_statement<st>')
    @production('statement -> assignment_statement<st>')
    def statement(self, st):
        return st

    @production('compound_statement<pos> -> block_start statement_or_declaration_list<statements> block_end<symbols>')
    @production('compound_statement<pos> -> block_start block_end<symbols>')
    def compound_statement(self, pos, symbols, statements=()):
        symbols.setType(SymbolScope.INHERITED)
        return CompoundStatementNode(statements=list(statements), symbols=symbols, pos=pos)

    @production('statement_or_declaration_list -> statement<stdecl>')
    @production('statement_or_declaration_list -> var_declaration<stdecl>')
    def statement_or_declaration_list_initial(self, stdecl):
        return (stdecl,)

    @production('statement_or_declaration_list -> statement_or_declaration_list<lst> statement<stdecl>')
    @production('statement_or_declaration_list -> statement_or_declaration_list<lst> var_declaration<stdecl>')
    def statement_or_declaration_list(self, lst, stdecl):
        return lst + (stdecl,)

    @production('expression_statement -> expression<expr> ";"')
    def expression_statement(self, expr):
        return expr

    @production('selection_statement<pos> -> IF "(" expression<expr> ")" statement<yes>')
    @production('selection_statement<pos> -> IF "(" expression<expr> ")" statement<yes> ELSE statement<no>')
    def selection_statement(self, pos, expr, yes, no=None):
        try:
            return IfNode(expr=expr, yes=yes, no=no or EmptyNode(pos=pos), pos=pos)
        except CastError as exc:
            self._errors.append((str(exc), pos))
            return IfNode(expr=ConstantNode(value=0, pos=pos), yes=yes, no=no or EmptyNode(pos=pos), pos=pos)

    @production('iteration_statement<pos> -> WHILE "(" expression<expr> ")" statement<body>')
    def iteration_statement(self, pos, expr, body):
        try:
            return WhileNode(expr=expr, body=body, pos=pos)
        except CastError as exc:
            self._errors.append((str(exc), pos))
            return WhileNode(expr=ConstantNode(value=0, pos=pos), body=body, pos=pos)

    @production('jump_statement<pos> -> CONTINUE ";"')
    def jump_statement_continue(self, pos):
        return ContinueNode(pos=pos)

    @production('jump_statement<pos> -> BREAK ";"')
    def jump_statement_break(self, pos):
        return BreakNode(pos=pos)

    @production('jump_statement<pos> -> YIELD ";"')
    def jump_statement_yield(self, pos):
        return YieldNode(pos=pos)

    @production('jump_statement<pos> -> GO IDENTIFIER<state> ";"')
    def jump_statement_go(self, pos, state):
        return GoNode(target=state, pos=pos)

    @production('jump_statement<pos> -> RETURN ";"')
    @production('jump_statement<pos> -> RETURN expression<expr> ";"')
    def jump_statement_return(self, pos, expr=None):
        return ReturnNode(expr=expr or EmptyNode(pos=pos), pos=pos)

    @production('assignment_statement<pos> -> postfix_expression<target> assignment_operator<op> expression<expr> ";"')
    def assignment_statement(self, pos, target, op, expr):
        if not target.isLValue():
            self._errors.append(('assignment target is not an lvalue', pos))

        if not expr.isRValue():
            self._errors.append(('assignment expression is not an rvalue', pos))
            expr = ConstantNode(value=0, pos=pos)

        try:
            return AssignmentNode(target=target, op=op, expr=expr, pos=pos)
        except CastError as exc:
            self._errors.append((str(exc), pos))
            return EmptyNode(pos=pos)

    @production('parameter_declaration<pos> -> type_specifier<type_> IDENTIFIER<name>')
    def parameter_declaration(self, pos, type_, name):
        if type_.name == 'void':
            self._errors.append(('cannot declare parameter of type "void"', pos))
            type_ = INT
        return ArgumentNode(name=name, type_=type_, pos=pos)

    @production('callable_declaration -> type_specifier<rettype> IDENTIFIER<name> "(" parameter_declaration*(",")<args> ")"')
    def callable_declaration(self, rettype, name, args):
        self._symbols.push()
        for arg in args:
            self._symbols.add(arg.name, arg)
        return rettype, name, args

    @production('callable_definition -> callable_declaration<decl> compound_statement<body>')
    def callable_definition(self, decl, body):
        symbols = self._symbols.pop()
        symbols.setType(SymbolScope.ARGS)
        rettype, name, args = decl
        symbols.addSize(rettype.size)
        return rettype, name, args, body, symbols

    @production('function_definition<pos> -> callable_definition<func>')
    def function_definition(self, pos, func):
        rettype, name, args, body, symbols = func
        node = FunctionNode(rettype=rettype, name=name, args=args, body=body, symbols=symbols, pos=pos)
        self._symbols.add(name, node)
        return node

    @production('method_definition<pos> -> callable_definition<func>')
    def method_definition(self, pos, func):
        rettype, name, args, body, symbols = func
        symbols.addSize(4) # %TH on the stack
        node = MethodNode(rettype=rettype, name=name, args=args, body=body, symbols=symbols, pos=pos)
        self._symbols.add(name, node)
        return node

    @production('state_method_declaration -> IDENTIFIER<name> "(" ")"')
    def state_method_declaration(self, name):
        self._symbols.push()
        return name

    @production('state_method_definition<pos> -> state_method_declaration<name> compound_statement<body>')
    def state_method_definition(self, pos, name, body):
        symbols = self._symbols.pop()
        symbols.setType(SymbolScope.LOCAL)
        node = StateMethodNode(name=name, body=body, symbols=symbols, pos=pos)
        self._symbols.add(name, node)
        return node


class Compiler(Preprocessor, Parser):
    def __init__(self):
        super().__init__(comment='//', directive='#')
        self._preprocessed = io.StringIO()

    def preprocessed(self, line):
        self._preprocessed.write('%s\n' % line)

    def compile(self):
        return self.parse(self._preprocessed.getvalue())
