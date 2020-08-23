#!/usr/bin/env python3

import io
import struct

from .compiler import Compiler
from .interm import ICGenerator, Label
from .codegen import CodeGenerator, StackSize


class BytecodeGenError(Exception):
    def __init__(self, errors):
        super().__init__()
        self.errors = errors

    def __str__(self):
        return ', '.join([msg for msg, _ in self.errors])


def generateBytecode(source): # pylint: disable=R0914
    comp = Compiler()
    ppwarnings = comp.preprocess(io.StringIO(source))
    ast, warnings, errors = comp.compile()
    warnings += ppwarnings
    if errors:
        raise BytecodeGenError(errors)

    gen = ICGenerator()
    ops = gen.generate(ast)

    gen = CodeGenerator()
    ops = gen.generate(ops)
    size = StackSize().visit(ast)

    bytecode = io.BytesIO()
    bytecode.write(struct.pack('<H', size))

    offsets = {}
    labels = {}
    for op in ops:
        if isinstance(op, Label):
            labels[op] = bytecode.tell() - 2
        else:
            op.write(offsets, bytecode)

    for label, deltas in offsets.items():
        for delta in deltas:
            bytecode.seek(delta, io.SEEK_SET)
            bytecode.write(struct.pack('<H', labels[label]))
    bytecode.seek(0, io.SEEK_END)
    return warnings, bytecode
