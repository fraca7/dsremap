#!/usr/bin/env python3

import sys
import re

from ptk.lexer import LexerPosition


class PreprocessorWarning(Exception):
    pass


class Preprocessor:
    def __init__(self, *args, comment=';', directive='#', **kwargs):
        super().__init__(*args, **kwargs)
        self._macros = {}
        self._comment = comment
        self._directive = directive

    def preprocessed(self, line):
        raise NotImplementedError

    def addMacro(self, name, value=None):
        exists = name in self._macros
        self._macros[name] = (self.expandMacros(value), re.compile(r'(?:\W|\A)(%s)(?:\W|$)' % name))
        if exists:
            raise PreprocessorWarning('macro "%s" redefined' % name)

    def removeMacro(self, name):
        if name in self._macros:
            del self._macros[name]
            return
        raise PreprocessorWarning('macro "%s" is not defined' % name)

    def expandMacros(self, value):
        changed = True
        while changed:
            changed = False
            for val, regex in self._macros.values():
                match = regex.search(value)
                if match is not None:
                    changed = True
                    value = '%s%s%s' % (value[:match.start(1)], val, value[match.end(1):])
                    break
        return value

    def preprocess(self, src): # pylint: disable=R0912,R0915
        lineno = 0
        stack = [True]
        warnings = []

        for line in src:
            lineno += 1

            index = line.find(self._comment)
            if index != -1:
                line = line[:index]
            line = line.rstrip()
            sline = line.strip()

            if sline.startswith('%selse' % self._directive):
                stack[-1] = not stack[-1]
                self.preprocessed('')
                continue

            if sline.startswith('%sendif' % self._directive):
                stack.pop()
                self.preprocessed('')
                continue

            if sline.startswith('%sifdef' % self._directive):
                name = sline[6:].strip()
                stack.append(name in self._macros)
                self.preprocessed('')
                continue

            if sline.startswith('%sifndef' % self._directive):
                name = sline[7:].strip()
                stack.append(not name in self._macros)
                self.preprocessed('')
                continue

            if not all(stack):
                self.preprocessed('')
                continue

            if sline.startswith('%sdefine' % self._directive):
                sline = sline[7:].strip()
                match = re.search(r'\s', sline)
                if match is None:
                    name = sline
                    value = None
                else:
                    index = match.start()
                    name, value = sline[:index], sline[index:].strip()

                try:
                    self.addMacro(name, value)
                except PreprocessorWarning as exc:
                    warnings.append((str(exc), LexerPosition(line=lineno, column=1)))

                self.preprocessed('')
                continue

            if sline.startswith('%sundef' % self._directive):
                name = sline[6:].strip()
                try:
                    self.removeMacro(name)
                except PreprocessorWarning as exc:
                    warnings.append((str(exc), LexerPosition(line=lineno, column=1)))

                self.preprocessed('')
                continue

            self.preprocessed(self.expandMacros(line))

        return warnings


class FilePreprocessor(Preprocessor):
    def __init__(self, dst, **kwargs):
        super().__init__(**kwargs)
        self._dst = dst

    def preprocessed(self, line):
        self._dst.write('%s\n' % line)


def main(argv):
    with open(argv[0], 'r') as fileobj:
        preproc = FilePreprocessor(fileobj)
        warnings = preproc.preprocess(sys.stdout)

    for warning, pos in warnings:
        sys.stderr.write('At line %d: %s\n' % (pos.line, warning))

if __name__ == '__main__':
    main(sys.argv[1:])
