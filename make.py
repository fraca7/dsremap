#!/usr/bin/env python3

import sys
import os
import functools
import codecs
import contextlib
import platform
import subprocess

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

from dsrlib.meta import Meta

@contextlib.contextmanager
def chdir(*args):
    old = os.getcwd()
    try:
        os.chdir(os.path.join(*args))
        yield
    finally:
        os.chdir(old)


def depends(*tasks, done=set()): # Yes, mutable default.
    def wrap(func):
        @functools.wraps(func)
        def wrapper():
            for task in tasks:
                if task not in done:
                    task()
                    done.add(task)
            func()
        return wrapper
    return wrap

# Generated files for C++

def headers():
    # Log messages
    from tools.genids import main
    main([
        '-o', os.path.join('src', 'arduino', 'dsremap', 'messages.h'),
        '-s', os.path.join('tools', 'strings.pickle'),
        os.path.join('src', 'arduino', 'dsremap', 'messages.txt'),
        ])

    # Opcodes
    from dsrlib.compiler.opcodes import export
    export(os.path.join('src', 'arduino', 'dsremap', 'opcodes.h'))

    # Version
    with codecs.getwriter('utf-8')(open(os.path.join('src', 'arduino', 'dsremap', 'version.h'), 'wb')) as fileobj:
        from dsrlib.meta import Meta
        version = Meta.firmwareVersion()

        fileobj.write('#ifndef _DSREMAP_VERSION_H_\n')
        fileobj.write('#define _DSREMAP_VERSION_H_\n')
        fileobj.write('#define FW_VERSION_MAJOR %d\n' % version.major)
        fileobj.write('#define FW_VERSION_MINOR %d\n' % version.minor)
        fileobj.write('#define FW_VERSION_PATCH %d\n' % version.patch)
        fileobj.write('#endif\n')


# Generated files for Python

def resources():
    path = 'res'
    with codecs.getwriter('utf-8')(open(os.path.join(path, 'resources.qrc'), 'wb')) as fileobj:
        fileobj.write('<!DOCTYPE RCC>\n')
        fileobj.write('<RCC version="1.0">\n')
        fileobj.write('  <qresource>\n')
        fileobj.write('    <file>about.html</file>\n')
        for dirpath, dirnames, filenames in os.walk(path):
            for filename in filenames:
                if filename.endswith('.svg') or filename.endswith('.jpg'):
                    fileobj.write('    <file>%s</file>\n' % os.path.join(os.path.relpath(dirpath, path), filename))
        fileobj.write('  </qresource>\n')
        fileobj.write('</RCC>\n')

    subprocess.run(['pyrcc5', '-o', os.path.join('src', 'dsrlib', 'resources.py'), os.path.join('res', 'resources.qrc')], check=True)

# Other generated files

def messages():
    from tools import pygettext
    pygettext.main([
        '-a',
        '-n',
        '-o', os.path.join('i18n', 'messages.pot'),
        os.path.join('src', 'dsrlib'),
        ])

# C++ extension, Arduino code

@depends(headers)
def ext():
    with chdir('src', 'ext'):
        subprocess.run([sys.executable, 'setup.py', 'build_ext', '-i'], check=True)


@depends(headers, resources)
def firmware():
    if platform.system() == 'Darwin':
        exe = '/Applications/Arduino.app/Contents/MacOS/Arduino'
    elif platform.system() == 'Linux':
        # Even Ubuntu 20.x only provides Arduino 1.x ?
        exe = os.path.expanduser('~/bin/arduino/arduino')
    elif platform.system() == 'Windows':
        import winreg
        try:
            key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r'SOFTWARE\Arduino', 0, winreg.KEY_READ|winreg.KEY_WOW64_32KEY)
            try:
                path, _ = winreg.QueryValueEx(key, 'Install_Dir')
            finally:
                winreg.CloseKey(key)
        except Exception as exc:
            raise RuntimeError('Cannot find Arduino: %s' % exc)
        # The "regular" arduino.exe does some strange shit and subprocess.run returns immediately, so...
        exe = os.path.join(path, 'arduino_debug.exe')
    else:
        raise RuntimeError('Unsupported platform')

    subprocess.run([exe, '--verify', '--board', 'arduino:avr:leonardo', '--verbose-build', '--pref', 'build.path=%s' % os.path.join(os.getcwd(), 'arduino-build'), os.path.join('src', 'arduino', 'dsremap', 'dsremap.ino')], check=True)


# Doc and tests

def manual():
    # XXXFIXME invoke Sphinx directly
    with chdir('doc'):
        ret = os.system('make html')
        if ret:
            raise RuntimeError('Cannot build documentation: %s' % ret)


@depends(resources)
def lint():
    from pylint.lint import Run
    Run([
        '--ignore=resources.py,opcodes.py',
        os.path.join('src', 'dsrlib'),
        ])


def regtests():
    from codetests.runtests import main
    with chdir('codetests'):
        main()


@depends(ext)
def unittests():
    with chdir('tests'):
        subprocess.run([sys.executable, 'test_all.py'], check=True)

# Frozen

@depends(resources, ext, firmware)
def exe():
    if platform.system() == 'Darwin':
        subprocess.run([sys.executable, 'setup.py', 'py2app'], check=True)

        import dmgbuild
        dmgbuild.build_dmg('dsremap-%s.dmg' % str(Meta.appVersion()), Meta.appName(), 'dmgbuild-settings.py')
    elif platform.system() == 'Windows':
        subprocess.run([sys.executable, 'setup.py', 'build'], check=True)
        distdir = r'build\exe.win-%s-%s' % (platform.machine().lower(), '%d.%d' % sys.version_info[:2])
        with codecs.getwriter('utf-8')(open('installer.nsi', 'wb')) as dst:
            with codecs.getreader('utf-8')(open('installer.nsi.in', 'rb')) as src:
                for line in src:
                    dst.write(line.replace('@APPNAME@', Meta.appName()).replace('@APPVERSION@', str(Meta.appVersion())).replace('@DISTDIR@', distdir).replace('@WEBSITE@', Meta.appSite()))
        import winreg
        key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r'SOFTWARE', 0, winreg.KEY_READ|winreg.KEY_WOW64_32KEY)
        try:
            path = winreg.QueryValue(key, 'NSIS')
        except:
            raise RuntimeError('Cannot find NSIS in registry, is it installed ?')
        finally:
            winreg.CloseKey(key)
        # Assuming NSIS 3.x here
        subprocess.run([os.path.join(path, 'makensis.exe'), '/INPUTCHARSET', 'UTF8', 'installer.nsi'], check=True)
    elif platform.system() == 'Linux':
        with codecs.getwriter('utf-8')(open('appimage.yml', 'wb')) as dst:
            with codecs.getreader('utf-8')(open('appimage.yml.in', 'rb')) as src:
                for line in src:
                    dst.write(line.replace('@VERSION@', str(Meta.appVersion())))
        subprocess.run(['appimage-builder', '--recipe', 'appimage.yml', '--skip-test'], check=True)
    else:
        raise RuntimeError('Unsupported platform')

# Shortcuts

@depends(resources, ext)
def prepare():
    pass


@depends(unittests, regtests)
def alltests():
    pass


@depends(prepare, manual, alltests, firmware)
def all():
    pass


def main(argv):
    for target in argv:
        globals()[target]()


if __name__ == '__main__':
    main(sys.argv[1:] or ['all'])
