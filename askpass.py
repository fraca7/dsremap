#!/usr/bin/env python3

import os
import sys
sys.path.insert(0, 'src')

# Fuck you sudo.
if os.path.basename(sys.argv[0]) == 'askpass':
    try:
        import dsrlib
    except ImportError:
        # Frozen. We were launched with the system's python.
        appdir = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..'))

        env = dict(os.environ)
        env['LD_LIBRARY_PATH'] = '{appdir}/lib/x86_64-linux-gnu:{appdir}/usr/lib/x86_64-linux-gnu:{appdir}/usr/local/lib/x86_64-linux-gnu:{appdir}/opt/libc/lib/x86_64-linux-gnu:{appdir}/opt/libc/usr/lib/x86_64-linux-gnu:{appdir}/opt/libc/usr/local/lib/x86_64-linux-gnu'.format(appdir=appdir)
        env['LD_PRELOAD'] = 'libapprun_hooks.so'
        env['LIBC_LIBRARY_PATH'] = '{appdir}/opt/libc/lib/x86_64-linux-gnu:{appdir}/opt/libc/usr/lib/x86_64-linux-gnu:{appdir}/opt/libc/usr/local/lib/x86_64-linux-gnu'.format(appdir=appdir)
        env['PYTHONHOME'] = '{appdir}/usr'.format(appdir=appdir)
        env['PYTHONPATH'] = '{appdir}/usr/lib/python3.8/site-packages'.format(appdir=appdir)
        os.execve('{appdir}/usr/bin/python3.8'.format(appdir=appdir), ['python3.8', '{appdir}/usr/bin/askpass'.format(appdir=appdir)], env)

from dsrlib.ui import setup, askpass


if __name__ == '__main__':
    setup()
    askpass()
