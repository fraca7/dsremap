#!/usr/bin/env python3

from pyqtcmd import Command

from dsrlib.domain.mixins import WorkspaceMixin


class AddConfigurationCommand(WorkspaceMixin, Command):
    def __init__(self, *, workspace, configuration):
        super().__init__(workspace=workspace)
        self._configuration = configuration

    def do(self):
        self.configurations().addItem(self._configuration)

    def undo(self):
        self.configurations().removeItem(self._configuration)


class DeleteConfigurationsCommand(WorkspaceMixin, Command):
    def __init__(self, *, workspace, configurations):
        super().__init__(workspace=workspace)
        self._configurations = sorted([(self.configurations().indexOf(configuration), configuration) for configuration in configurations])

    def do(self):
        for _, configuration in self._configurations:
            self.configurations().removeItem(configuration)

    def undo(self):
        for index, configuration in self._configurations:
            self.configurations().addItem(configuration, index=index)
