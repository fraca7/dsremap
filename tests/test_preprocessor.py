#!/usr/bin/env python3

import io
import unittest

import base
from dsrlib.compiler import FilePreprocessor


class TestPreprocessor(unittest.TestCase):
    def setUp(self):
        self._target = io.StringIO()
        self._preprocessor = FilePreprocessor(self._target)

    def _expect(self, text):
        lines = [line.strip() for line in text.split('\n') if line.strip() != '']
        self.assertEqual(text, '\n'.join(lines))

    def test_ifdef_not_defined(self):
        self._preprocessor.preprocess(io.StringIO('#ifdef SPAM\nnope\n!endif'))
        self._expect('')

    def test_ifdef_defined(self):
        self._preprocessor.preprocess(io.StringIO('#define SPAM\n!ifdef SPAM\nyope\n!endif'))
        self._expect('yope')

    def test_ifdef_else_1(self):
        self._preprocessor.preprocess(io.StringIO('#ifdef SPAM\nnope\n!else\nyope\n!endif'))
        self._expect('nope')

    def test_ifdef_else_2(self):
        self._preprocessor.preprocess(io.StringIO('#define SPAM\n!ifdef SPAM\nnope\n!else\nyope\n!endif'))
        self._expect('nope')

    def test_nested_ifdef_1(self):
        self._preprocessor.preprocess(io.StringIO('#define SPAM\n!ifdef SPAM\n!ifdef FOO\nnope\n!else\nyope\n!endif\n!endif'))
        self._expect('yope')

    def test_undef(self):
        self._preprocessor.preprocess(io.StringIO('#define SPAM\n!ifdef SPAM\n1\n!endif\n!undef SPAM\n!ifdef SPAM\n2\n!endif'))
        self._expect('1')


if __name__ == '__main__':
    unittest.main()
