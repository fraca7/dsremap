#!/usr/bin/env python3

import zipfile

from PyQt5 import QtWidgets

from pyqtcmd import NeedsSelectionUICommandMixin

from dsrlib.domain.mixins import WorkspaceMixin
from dsrlib.domain.persistence import JSONExporter, JSONImporter
from dsrlib.ui.utils import getSaveFilename, getOpenFilename

from .base import UICommand


class ExportConfigurationUICommand(NeedsSelectionUICommandMixin, UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, text=_('Export selected configuration'), tip=_('Export the selected configuration as a zip file'), **kwargs)

    def should_be_enabled(self):
        return len(self.selection()) == 1 and super().should_be_enabled()

    def do(self):
        filename = getSaveFilename(self.mainWindow(), 'Export', 'zip')
        if filename is None:
            return

        try:
            with zipfile.ZipFile(filename, mode='w', compression=zipfile.ZIP_DEFLATED) as zipobj:
                exporter = JSONExporter(zipobj)
                exporter.write(self.selection()[0])
        except Exception as exc: # pylint: disable=W0703
            QtWidgets.QMessageBox.critical(self.mainWindow(), _('Export error'), _('Cannot export configuration:\n{error}').format(error=str(exc)))


class ImportConfigurationUICommand(WorkspaceMixin, UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, text=_('Import configuration'), tip=_('Import a previously exported configuration'), **kwargs)

    def do(self):
        filename = getOpenFilename(self.mainWindow(), 'Import', 'zip')
        if filename is None:
            return

        try:
            with zipfile.ZipFile(filename, mode='r') as zipobj:
                importer = JSONImporter()
                self.workspace().configurations().addItem(importer.read(zipobj))
        except Exception as exc: # pylint: disable=W0703
            QtWidgets.QMessageBox.critical(self.mainWindow(), _('Import error'), _('Cannot import configuration:\n{error}').format(error=str(exc)))


class ExportBytecodeUICommand(WorkspaceMixin, UICommand):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, text=_('Export bytecode'), tip=_('Export bytecode to file'), **kwargs)

    def do(self):
        filename = getSaveFilename(self.mainWindow(), 'ExportBC', 'bin')
        if filename is None:
            return

        try:
            with open(filename, 'wb') as fileobj:
                fileobj.write(self.workspace().bytecode())
        except Exception as exc: # pylint: disable=W0703
            QtWidgets.QMessageBox.critical(self.mainWindow(), _('Export error'), _('Cannot export bytecode:\n{error}').format(error=str(exc)))
