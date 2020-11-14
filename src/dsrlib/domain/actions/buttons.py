#!/usr/bin/env python3

from dsrlib.domain.buttons import Buttons

from .base import Action


class DisableButtonAction(Action):
    def __init__(self):
        self._button = Buttons.L3
        super().__init__()

    def labelFormat(self):
        return _('Disable {button}')

    def labelValues(self):
        return {'button': Buttons.label(self._button)}

    def button(self):
        return self._button

    def setButton(self, button):
        if button != self._button:
            self._button = button
            self.notifyChanged()

    def source(self):
        return '''
state idle {
  idle() {
    %(BUTTONNAME)s = 0;
  }
};
        ''' % dict(BUTTONNAME=self._button.name)
