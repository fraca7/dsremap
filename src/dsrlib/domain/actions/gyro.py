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
#define FPS 30
#define PPD_X (1920 / 10)
#define PPD_Y (1080 / 10)
#define DEADZONE 50

#define MAX_SPEED 4.4
#define MIN_SPEED MAX_SPEED / 8

struct AxisState {
  float origin;
  float current;
  float target;
  float speed;
  int padval;

  void init(float angle) {
    origin = angle;
    current = 0;
    target = 0;
    speed = 0;
    padval = 0x80;
  }

  void integrate() {
    current = current + speed * DELTA;
  }

  void compute(float angle, int ppd) {
    target = 1000.0 * (angle - origin) * ppd;

    float delta = target - current;
    int sign = 1;
    if (delta < 0) {
      delta = -delta;
      sign = -1;
    }

    if (delta < 1) {
      speed = 0;
    } else {
      speed = FPS * delta / 1000000.0;
      if (speed > MAX_SPEED)
        speed = MAX_SPEED;
      if (speed < MIN_SPEED)
        speed = MIN_SPEED;
    }

    speed = sign * speed;
    padval = 128 * speed / MAX_SPEED + 0x80;
  }
};

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
  AxisState stateX;
  AxisState stateY;

  enter() {
    stateX.init(IMUY);
    stateY.init(IMUX);
  }

  gyro_aiming() {
    if (!should_aim()) {
      go idle;
    }

    if (rpad_delta() >= DEADZONE) {
      go manual_aiming;
    }

    stateX.compute(IMUY, PPD_X);
    stateY.compute(IMUX, PPD_Y);

    int count = 0;
    while (count++ * 5 <= 1000 / FPS) {
      stateX.integrate();
      RPadX = stateX.padval;

      stateY.integrate();
      RPadY = stateY.padval;

      yield;
    }
  }
};
""" % dict(BUTTONCOND=' && '.join([button.name for button in self._buttons]))
