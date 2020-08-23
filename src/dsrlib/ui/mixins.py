#!/usr/bin/env python3

from dsrlib.settings import Settings


class MainWindowMixin:
    def __init__(self, *args, mainWindow, **kwargs):
        self._mainWindow = mainWindow
        super().__init__(*args, **kwargs)

    def mainWindow(self):
        return self._mainWindow

    def history(self):
        return self._mainWindow.history()


class DeveloperModeItemMixin:
    # Should be used only in conjunction with MainWindowMixin
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.mainWindow().settingsChanged.connect(self._onSettingsChanged)
        self._onSettingsChanged()

    def _onSettingsChanged(self):
        with Settings().grouped('Settings', 'Misc') as settings:
            self.setVisible(settings.booleanValue('devmode', False))
