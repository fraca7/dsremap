#!/usr/bin/env python3


class ConfigurationMixin:
    def __init__(self, *args, configuration, **kwargs):
        self._configuration = configuration
        super().__init__(*args, **kwargs)

    def configuration(self):
        return self._configuration

    def actions(self):
        return self._configuration.actions()


class WorkspaceMixin:
    def __init__(self, *args, workspace, **kwargs):
        self._workspace = workspace
        super().__init__(*args, **kwargs)

    def workspace(self):
        return self._workspace

    def configurations(self):
        return self._workspace.configurations()
