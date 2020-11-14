#!/usr/bin/env python3

import os
import sys
import locale
import gettext
import logging
import platform
import subprocess
import glob
import zipfile
import getopt

from PyQt5 import QtCore, QtGui, QtWidgets, QtNetwork

from dsrlib import resources # pylint: disable=W0611
from dsrlib.meta import Meta
from dsrlib.settings import Settings
from dsrlib.domain import Workspace, DeviceEnumerator, JSONImporter, Changelog
from dsrlib.ui.hexuploader import FirstLaunchWizard
from dsrlib.ui.changelog import ChangelogView
from dsrlib.ui.utils import LayoutBuilder

from dsrlib.domain.sudo import sudoLaunch, SudoNotFound, SudoError
from dsrlib.ui import uicommands
from dsrlib.ui.workspace import WorkspaceView


def usage(msg=None, code=1):
    if msg:
        print(msg)
    print('Usage: %s [options]' % sys.argv[0])
    print('Options:')
    print('-h, --help         Display this')
    print('-n, --nuke         Erase configuration before starting up')
    sys.exit(code)


class Application(QtWidgets.QApplication):
    def __init__(self, argv):
        super().__init__([])
        self.setApplicationName(Meta.appName())
        self.setApplicationVersion(str(Meta.appVersion()))
        self.setOrganizationDomain(Meta.appDomain())
        self.setWindowIcon(QtGui.QIcon(':icons/gamepad.svg'))

        try:
            opts, args = getopt.getopt(sys.argv[1:], 'hn', ['help', 'nuke'])
        except getopt.GetoptError as exc:
            usage(str(exc))

        for opt, val in opts:
            if opt in ('-h', '--help'):
                usage(code=0)
            if opt in ('-n', '--nuke'):
                settings = QtCore.QSettings()
                settings.clear()

        # Builtin messages.
        trans = QtCore.QTranslator(self)
        path = QtCore.QLibraryInfo.location(QtCore.QLibraryInfo.TranslationsPath)
        trans.load(QtCore.QLocale.system(), 'qtbase', '_', path)
        self.installTranslator(trans)



class MainWindow(QtWidgets.QMainWindow):
    settingsChanged = QtCore.pyqtSignal()

    def __init__(self):
        super().__init__()

        self._manager = QtNetwork.QNetworkAccessManager(self)

        self._workspace = Workspace()
        self.setCentralWidget(WorkspaceView(self, mainWindow=self, workspace=self._workspace))
        self._workspace.load()
        self._devenum = DeviceEnumerator()

        self.setWindowTitle(_('{appName} v{appVersion}').format(appName=Meta.appName(), appVersion=str(Meta.appVersion())))
        self.statusBar()

        filemenu = QtWidgets.QMenu(_('File'), self)
        filemenu.addAction(uicommands.ExportConfigurationUICommand(self, mainWindow=self, container=self.centralWidget()))
        filemenu.addAction(uicommands.ImportConfigurationUICommand(self, mainWindow=self, workspace=self._workspace))
        filemenu.addAction(uicommands.ExportBytecodeUICommand(self, mainWindow=self, workspace=self._workspace))
        self.menuBar().addMenu(filemenu)

        editmenu = QtWidgets.QMenu(_('Edit'), self)
        editmenu.addAction(uicommands.UndoUICommand(self, mainWindow=self))
        editmenu.addAction(uicommands.RedoUICommand(self, mainWindow=self))
        editmenu.addAction(uicommands.ShowSettingsDialogUICommand(self, mainWindow=self))
        self.menuBar().addMenu(editmenu)

        devmenu = uicommands.DeviceMenu(self, mainWindow=self, workspace=self._workspace, enumerator=self._devenum)
        self.menuBar().addMenu(devmenu)

        helpmenu = QtWidgets.QMenu(_('Help'), self)
        helpmenu.addAction(uicommands.ShowAboutDialogUICommand(self, mainWindow=self))
        helpmenu.addAction(uicommands.OpenDocsUICommand(self, mainWindow=self))
        self.menuBar().addMenu(helpmenu)

        with Settings().grouped('UIState') as settings:
            if settings.contains('WindowGeometry'):
                self.restoreGeometry(settings.value('WindowGeometry'))
            else:
                self.resize(1280, 600)
            self.centralWidget().loadState(settings)

        self.raise_()
        self.show()

        settings = Settings()
        if settings.isFirstVersionLaunch():
            QtGui.QDesktopServices.openUrl(QtCore.QUrl(Meta.documentationUrl()))

        self.check()

        self._changelogReply = self._manager.get(QtNetwork.QNetworkRequest(QtCore.QUrl(Meta.changelogUrl())))
        self._changelogReply.finished.connect(self._onChangelogReply)

    def history(self):
        return self._workspace.history()

    def manager(self):
        return self._manager

    def closeEvent(self, event):
        self._workspace.save()
        self._workspace.cleanup()
        self._devenum.stop()

        if self._changelogReply:
            self._changelogReply.abort()

        with Settings().grouped('UIState') as settings:
            settings.setValue('WindowGeometry', self.saveGeometry())
            self.centralWidget().saveState(settings)
        event.accept()

    def reloadSettings(self):
        pass

    def check(self): # pylint: disable=R0912,R0914,R0915
        if platform.system() == 'Linux':
            # 1. plugdev group

            groupOk = False
            retcode = 0
            try:
                proc = subprocess.Popen(['groups'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                outData, unused = proc.communicate()
            except Exception as exc: # pylint: disable=W0703
                msg = str(exc)
                retcode = 1
            else:
                retcode = proc.returncode
                msg = _('<i>groups</i> exited with code {code}').format(code=proc.returncode)

            if retcode:
                QtWidgets.QMessageBox.warning(self, _('Cannot find groups'), _('Unable to find out which groups you belong to:<p />{error}<p />For this program to work correctly, you must belong to the <i>plugdev</i> group.').format(error=msg))
                groupOk = True
            else:
                groups = Meta.decodePlatformString(outData).split()
                groupOk = 'plugdev' in groups

            # 2. udev rules
            udevOk = os.path.exists('/etc/udev/rules.d/80-dsremap.rules')

            if not (udevOk and groupOk):
                msg = _('The following operations must be done before this program can work correctly:') + '<p /><ul>'
                if not udevOk:
                    msg += '<li>' + _('Install udev rules to allow HID communication') + '</li>'
                if not groupOk:
                    msg += '<li>' + _('Add you to the <i>plugdev</i> group') + '</li>'
                msg += '</ul>'
                if not groupOk:
                    msg += '<p />' + _('You may have to logout and login again, or even reboot, and unplug/replug the Arduino for those changes to apply.')

                msg += '<p />' + _('You may be prompted for your password.')

                QtWidgets.QMessageBox.information(self, _('System setup'), msg)

                try:
                    if not groupOk:
                        sudoLaunch('usermod', '-a', '-G', 'plugdev', os.environ['USER'])
                    if not udevOk:
                        # Cannot copy a file from the res directory because root hasn't the permissions on the mount dir...
                        sudoLaunch('sh', '-c', 'echo \'SUBSYSTEM=="usb", ATTRS{idVendor}=="054c", ATTRS{idProduct}=="09cc", GROUP="plugdev", MODE="0660"\' > /etc/udev/rules.d/80-dsremap.rules')
                        sudoLaunch('sh', '-c', 'echo \'SUBSYSTEM=="tty", ATTRS{idVendor}=="2341", ATTRS{idProduct}=="8036", GROUP="plugdev", MODE="0660"\' >> /etc/udev/rules.d/80-dsremap.rules')
                except SudoNotFound:
                    QtWidgets.QMessageBox.critical(self, _('Error'), _('Unable to find <i>sudo</i> on the path.'))
                except SudoError as exc:
                    QtWidgets.QMessageBox.critical(self, _('Error'), _('Command failed with exit code {code}.<p />{stderr}').format(code=exc.code, stderr=exc.stderr))

        with Settings().grouped('DefaultConfigurations') as settings:
            imported = settings.value('Imported').split(',') if settings.contains('Imported') else []
            importer = JSONImporter()
            for name in glob.glob(os.path.join(Meta.defaultConfigurations(), '*.zip')):
                with zipfile.ZipFile(name, mode='r') as zipobj:
                    configuration = importer.read(zipobj)
                if configuration.uuid() not in imported:
                    imported.append(configuration.uuid())
                    self._workspace.configurations().addItem(configuration)
            settings.setValue('Imported', ','.join(imported))

        if not Settings().firmwareUploaded():
            wizard = FirstLaunchWizard(self)
            wizard.exec_()

    def _onChangelogReply(self):
        reply, self._changelogReply = self._changelogReply, None
        try:
            logger = logging.getLogger('dsremap')
            status = reply.attribute(QtNetwork.QNetworkRequest.HttpStatusCodeAttribute)
            if status != 200:
                logger.warning('Got status %s while downloading changelog', status)
                return
            changelog = Changelog(bytes(reply.readAll()).decode('utf-8'))

            if changelog.changesSince(Meta.appVersion()):
                win = ChangelogView(self, changelog)
                win.show()
                win.raise_()
        except: # pylint: disable=W0702
            logger.exception('Cannot download changelog')


class PasswordDialog(QtWidgets.QDialog):
    def __init__(self):
        super().__init__()
        self.setWindowTitle(_('Password needed'))

        self._pwd = QtWidgets.QLineEdit(self)
        self._pwd.setEchoMode(self._pwd.Password)

        bld = LayoutBuilder(self)
        with bld.vbox() as vbox:
            vbox.setContentsMargins(5, 5, 5, 5)
            vbox.addWidget(QtWidgets.QLabel(_('You need to enter your password to setup groups and udev rules.'), self))
            vbox.addWidget(self._pwd)
            with bld.hbox() as hbox:
                hbox.addStretch(1)
                btn = QtWidgets.QPushButton(_('Cancel'), self)
                btn.clicked.connect(self.reject)
                hbox.addWidget(btn)
                btn = QtWidgets.QPushButton(_('OK'), self)
                btn.clicked.connect(self.accept)
                btn.setDefault(True)
                hbox.addWidget(btn)

    def accept(self):
        print(self._pwd.text())
        super().accept()


def setup():
    logging.TRACE = 5
    def trace(self, fmt, *args, **kwargs):
        if self.isEnabledFor(logging.TRACE):
            self._log(logging.TRACE, fmt, args, **kwargs) # pylint: disable=W0212
    logging.Logger.trace = trace
    logging.addLevelName(logging.TRACE, 'TRACE')
    logging.basicConfig(level=logging.INFO, format='%(asctime)-15s %(levelname)-8s %(name)-15s %(message)s')

    if platform.system() == 'Darwin':
        localeName = str(QtCore.QLocale.system().uiLanguages()[0]).replace('-', '_')
    else:
        localeName = str(QtCore.QLocale.system().name())

    try:
        locale.setlocale(locale.LC_ALL, localeName)
    except locale.Error as exc:
        logger = logging.getLogger('dsremap')
        logger.error('Cannot set locale to "%s": %s', localeName, exc)

    trans = gettext.translation(Meta.appName(), localedir=Meta.messages(), languages=[localeName], fallback=True)
    trans.install()


def uimain():
    app = Application(sys.argv[1:])
    win = MainWindow() # pylint: disable=W0612
    app.exec_()


def askpass():
    app = Application(sys.argv) # pylint: disable=W0612
    dlg = PasswordDialog()
    if dlg.exec_() == dlg.Accepted:
        sys.exit(0)
    sys.exit(1)
