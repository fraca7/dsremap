#!/usr/bin/env python3

from dsrlib.domain.buttons import Buttons

from .base import Action


class GyroAction(Action):
    def __init__(self):
        self._buttons = [Buttons.L2, Buttons.R2]
        super().__init__()

    def labelFormat(self):
        return _('Gyro aiming when pressing {buttons}')

    def labelValues(self):
        return {'buttons': '+'.join([Buttons.label(button) for button in self._buttons])}

    def buttons(self):
        return list(self._buttons)

    def setButtons(self, buttons):
        if buttons != self._buttons:
            self._buttons = list(buttons)
            self.notifyChanged()

    def source(self):
        return """
#define DEADZONE 50
#define ACCEL_FACTOR 5

int rpad_delta() {
  return (RPadX - 128) * (RPadX - 128) + (RPadY - 128) * (RPadY - 128);
}

int should_aim() {
  return %(BUTTONCOND)s;
}

state idle {
  idle() {
    if (should_aim()) {
      if (rpad_delta() < DEADZONE) {
        go gyro_aiming;
      } else {
        go manual_aiming;
      }
    }
  }
};

state manual_aiming {
  manual_aiming() {
    if (!should_aim()) {
      go idle;
    }

    if (rpad_delta() < DEADZONE) {
      go gyro_aiming;
    }
  }
};

state gyro_aiming {
  gyro_aiming() {
    if (!should_aim()) {
      go idle;
    }

    if (rpad_delta() >= DEADZONE) {
      go manual_aiming;
    }

    RPadX = ACCEL_FACTOR * ACCELY + 0x80;
    RPadY = ACCEL_FACTOR * ACCELX + 0x80;
  }
};
""" % dict(BUTTONCOND=' && '.join([button.name for button in self._buttons]))
