#!/usr/bin/env python3

import platform

from .base import FileManager

if platform.system() == 'Linux':
    from . import linux
if platform.system() == 'Darwin':
    from . import macos
if platform.system() == 'Windows':
    from . import windows
