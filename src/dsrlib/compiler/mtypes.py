#!/usr/bin/env python3


class CastError(Exception):
    pass


class TypeType:
    def __init__(self):
        self.size = 0


class Type:
    def __init__(self, name, size):
        self.name = name
        self.size = size

    def __str__(self):
        return self.name

    def __eq__(self, other):
        return self.name == other.name

    def type(self):
        return TypeType()

    def cast(self, expr):
        raise CastError('cannot cast %s to %s' % (self, expr.type()))

    def value(self, value):
        raise CastError('cannot cast %s to %s' % (self, value))

    @staticmethod
    def maxType(*types):
        if len(types) == 1:
            return types[0]
        if len(types) == 2:
            type1, type2 = types
        else:
            type1, type2 = types[0], Type.maxType(*types[1:])

        if type1 == type2:
            return type1

        if (type1, type2) == (INT, INT):
            return INT
        if (type1, type2) == (INT, FLOAT):
            return FLOAT
        if (type1, type2) == (FLOAT, INT):
            return FLOAT
        if (type1, type2) == (FLOAT, FLOAT):
            return FLOAT

        raise CastError('incompatible types %s and %s' % (type1, type2))


class _VOID(Type):
    def __init__(self):
        super().__init__('void', 0)
VOID = _VOID()

class _INT(Type):
    def __init__(self):
        super().__init__('int', 2)

    def cast(self, expr):
        if expr.type() == INT:
            return None
        if expr.type() == FLOAT:
            return 'casti'
        raise CastError('cannot convert %s to int' % expr.type())

    def value(self, value):
        return int(value)

INT = _INT()


class _FLOAT(Type):
    def __init__(self):
        super().__init__('float', 4)

    def cast(self, expr):
        if expr.type() == FLOAT:
            return None
        if expr.type() == INT:
            return 'castf'
        raise CastError('cannot convert %s to float' % expr.type())

    def value(self, value):
        return float(value)

FLOAT = _FLOAT()


class StructType(Type):
    def __init__(self, name, members, methods, symbols):
        self.members = members
        self.methods = methods
        self.symbols = symbols

        size = 0
        for var in self.members:
            size += var.type_.size

        super().__init__(name, size)

    def hasMember(self, name):
        for member in self.members + self.methods:
            if member.name == name:
                return True
        return False

    def memberType(self, name):
        for member in self.members + self.methods:
            if member.name == name:
                return member.type()
        raise AttributeError(name) # Should not happen actually


class StateType(Type):
    def __init__(self, init, main, symbols):
        self.init = init
        self.main = main
        self.symbols = symbols
        super().__init__('state', 0)


class CallableType(Type):
    def __init__(self, rettype, args):
        self.rettype = rettype
        self.args = args
        super().__init__('function', 0)

    def argsize(self):
        size = 0
        for arg in self.args:
            size += arg.type().size
        return size

    def savesize(self):
        return 0


class FunctionType(CallableType):
    pass


class MethodType(CallableType):
    def savesize(self):
        return super().savesize() + 2


class StateMethodType(CallableType):
    def __init__(self):
        super().__init__(VOID, [])
