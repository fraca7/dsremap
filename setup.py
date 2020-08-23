#!/usr/bin/env python3

import os
import sys
import platform
import glob

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src', 'ext'))

from dsrlib.meta import Meta

if platform.system() == 'Darwin':
    from setuptools import setup

    data_files = [
          ('', ['res/avrdude.conf']),
          ('', ['arduino-build/dsremap.ino.hex']),
          ('', ['i18n']),
          ('configurations', glob.glob('res/configurations/*.zip')),
        ]

    setup(
        app=['dsremap.py'],
        options={'py2app': {'iconfile': os.path.join('icons', 'dsremap.icns')}},
        setup_requires=['py2app'],
        data_files=data_files
        )

if platform.system() == 'Windows':
    from cx_Freeze import setup, Executable

    options = {
        'include_files': [
            (r'res\avrdude.conf', r'resources\avrdude.conf'),
            (r'arduino-build\dsremap.ino.hex', r'resources\dsremap.ino.hex'),
            (r'res\configurations', r'resources\configurations'),
            (r'i18n', r'resources\i18n'),
            ]
    }

    setup(
        name=Meta.appName(),
        version=str(Meta.appVersion()),
        description=Meta.appName(),
        options={'build_exe': options},
        executables=[Executable('dsremap.py', base='Win32GUI', icon=r'icons\dsremap.ico')]
        )
