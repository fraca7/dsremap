#!/usr/bin/env python3

from enum import Enum, unique, auto


@unique
class Buttons(Enum):
    Square = auto()
    Cross = auto()
    Circle = auto()
    Triangle = auto()
    L1 = auto()
    R1 = auto()
    L2 = auto()
    R2 = auto()
    L3 = auto()
    R3 = auto()
    Share = auto()
    Options = auto()
    PS = auto()
    TPAD = auto()

    @classmethod
    def label(cls, button):
        return {
            cls.Square: _('\u25a1'),
            cls.Cross: _('\u00d7'),
            cls.Triangle: _('\u25b3'),
            cls.Circle: _('\u25cb'),
            cls.L1: _('L1'),
            cls.R1: _('R1'),
            cls.L2: _('L2'),
            cls.R2: _('R2'),
            cls.L3: _('L3'),
            cls.R3: _('R3'),
            cls.Share: _('Share'),
            cls.Options: _('Options'),
            cls.PS: _('PS'),
            cls.TPAD: _('TPAD'),
            }[button]
