#!/usr/bin/env python3

from .base import Action


class CustomAction(Action):
    def __init__(self):
        self._code = ''
        self._error = None
        super().__init__()

    def labelFormat(self):
        return _('Custom action: {line}')

    def labelValues(self):
        for line in self._code.split('\n'):
            if line.strip().startswith('//'):
                text = line.strip().lstrip('/').strip()
                break
        else:
            text = _('empty')

        return {'line': text}

    def source(self):
        return self._code

    def setSource(self, code):
        if code != self._code:
            self._code = code
            self.notifyChanged()

    def error(self):
        return self._error

    def notifyChanged(self):
        self._error = None
        try:
            super().notifyChanged()
        except Exception as exc: # pylint: disable=W0703
            self._error = str(exc)
            self._bytecode = b''
            self.changed.emit()
