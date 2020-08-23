#!/usr/bin/env python3

import os
import platform
from distutils.core import setup, Extension


opts = []
macros = []

if platform.system() == 'Linux':
    opts.append('-Wno-write-strings')
elif platform.system() == 'Darwin':
    opts.append('-Wno-c++11-compat-deprecated-writable-strings')
elif platform.system() == 'Windows':
    macros.append(('WIN32', '1'))

base_path = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..'))
ext = Extension('vmwrapper',
                sources=['vmwrapper.cpp', os.path.join(base_path, 'src', 'dsremap', 'VM.cpp')],
                include_dirs=[os.path.join(base_path, 'src', 'dsremap')],
                define_macros=[('TARGET_PC', 1), ('DEBUG', 1)] + macros,
                undef_macros=['NDEBUG'],
                extra_compile_args=opts)

setup(name='vmwrapper', version='1.0', description='VM wrapper module', ext_modules=[ext])
