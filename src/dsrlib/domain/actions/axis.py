#!/usr/bin/env python3

from enum import Enum, unique, auto

from .base import Action


class InvertPadAxisAction(Action):
    LPAD = 0
    RPAD = 1

    XAXIS = 0
    YAXIS = 1

    def __init__(self):
        self._pad = 0
        self._axis = 0
        super().__init__()

    def pad(self):
        return self._pad

    @classmethod
    def padLabel(cls, pad):
        return {cls.LPAD: _('left pad'), cls.RPAD: _('right pad')}[pad]

    @classmethod
    def axisLabel(cls, axis):
        return {cls.XAXIS: _('X axis'), cls.YAXIS: _('Y axis')}[axis]

    def axis(self):
        return self._axis

    def setPad(self, pad):
        if pad != self._pad:
            self._pad = pad
            self.notifyChanged()

    def setAxis(self, axis):
        if axis != self._axis:
            self._axis = axis
            self.notifyChanged()

    def labelFormat(self):
        return _('Invert {pad} {axis}')

    def labelValues(self):
        return {'pad': self.padLabel(self._pad), 'axis': self.axisLabel(self._axis)}

    def source(self):
        return """
#define PAD %(pad)s%(axis)s

state idle {
  idle() {
    PAD = 255 - PAD;
  }
};
""" % dict(pad={self.LPAD: 'LPad', self.RPAD: 'RPad'}[self._pad], axis={self.XAXIS: 'X', self.YAXIS: 'Y'}[self._axis])


@unique
class Axis(Enum):
    LPadX = auto()
    LPadY = auto()
    RPadX = auto()
    RPadY = auto()
    L2Value = auto()
    R2Value = auto()

    @classmethod
    def label(cls, axis):
        return {
            cls.LPadX: _('left pad X'),
            cls.LPadY: _('left pad Y'),
            cls.RPadX: _('right pad X'),
            cls.RPadY: _('right pad Y'),
            cls.L2Value: _('L2 value'),
            cls.R2Value: _('R2 value'),
            }[axis]


class SwapAxisAction(Action):
    def __init__(self):
        self._axis1 = Axis.LPadX
        self._axis2 = Axis.RPadX
        super().__init__()

    def axis(self):
        return self._axis1, self._axis2

    def axis1(self):
        return self._axis1

    def setAxis1(self, axis):
        if axis != self._axis1:
            self._axis1 = axis
            self.notifyChanged()

    def axis2(self):
        return self._axis2

    def setAxis2(self, axis):
        if axis != self._axis2:
            self._axis2 = axis
            self.notifyChanged()

    def labelFormat(self):
        return _('Swap {axis1} and {axis2}')

    def labelValues(self):
        return {'axis1': Axis.label(self._axis1), 'axis2': Axis.label(self._axis2)}

    def source(self):
        return """
state idle {
  idle() {
    int tmp = %(axis1)s;
    %(axis1)s = %(axis2)s;
    %(axis2)s = tmp;
  }
};
        """ % dict(axis1=self._axis1.name, axis2=self._axis2.name)
