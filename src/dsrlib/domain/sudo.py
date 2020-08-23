#!/usr/bin/env python3

import sys
import os
import subprocess

from dsrlib.meta import Meta


class SudoNotFound(Exception):
    pass


class SudoError(Exception):
    def __init__(self, code, errData):
        self.code = code
        self.stderr = errData
        super().__init__()


def sudoLaunch(*args):
    for path in os.environ['PATH'].split(os.pathsep):
        sudo = os.path.join(path, 'sudo')
        if os.path.exists(sudo):
            break
    else:
        raise SudoNotFound()

    if Meta.isFrozen():
        askpass = os.path.join(os.environ['APPDIR'], 'usr', 'bin', 'askpass')
    else:
        askpass = os.path.join(os.path.dirname(sys.argv[0]), 'askpass.py')

    env = dict(os.environ)
    env['SUDO_ASKPASS'] = askpass

    proc = subprocess.Popen([sudo, '-A'] + list(args), stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=env)
    unused, errData = proc.communicate()
    if proc.returncode:
        raise SudoError(proc.returncode, Meta.decodePlatformString(errData))
