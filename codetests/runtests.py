#!/usr/bin/env python3

import io
import sys
import codecs
import os
import difflib

from ptk.parser import ParseError
from ptk.lexer import LexerError

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from dsrlib.compiler import Compiler, ICGenerator, CodeGenerator


def run_test(basename):
    print('!! Running %s...' % basename)
    cc = Compiler()
    with codecs.getreader('utf-8')(open('%s.gac' % basename, 'rb')) as src:
        try:
            ppwarnings = cc.preprocess(src)
            ast, warnings, errors = cc.compile()
        except (LexerError, ParseError) as exc:
            print('!! At line %d, col %d: %s' % (exc.position.line, exc.position.column, exc))
            return False

        for msg, pos in ppwarnings + warnings:
            print('!! WARNING line %d: %s' % (pos.line, msg))
        for msg, pos in errors:
            print('!! ERROR line %d: %s' % (pos.line, msg))

        if ppwarnings or warnings or errors:
            return False

        gen = ICGenerator()
        ops = gen.generate(ast)

        gen = CodeGenerator()
        result = io.StringIO()
        for instruction in gen.generate(ops):
            result.write('%s\n' % str(instruction))

        rname = '%s.result' % basename
        if os.path.exists(rname):
            with codecs.getreader('utf-8')(open(rname, 'rb')) as fileobj:
                reference = fileobj.read().splitlines(keepends=True)
            result = result.getvalue().splitlines(keepends=True)

            differ = difflib.Differ()
            diff = list(differ.compare(reference, result))

            if any([line.startswith('+') or line.startswith('-') for line in diff]):
                print('!! Testing "%s"' % basename)
                for line in diff:
                    sys.stderr.write(line)
                return False

            return True

        print(result.getvalue())
        return True


def main():
    success = True
    path = os.path.dirname(__file__)
    count = 0
    for name in sorted(os.listdir(path)):
        if not name.endswith('.gac'):
            continue
        name, _ = os.path.splitext(name)
        success = success and run_test(os.path.join(path, name))
        count += 1

    print('Ran %d tests' % count)

    if not success:
        sys.exit(1)


if __name__ == '__main__':
    main()
