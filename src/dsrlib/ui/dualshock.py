#!/usr/bin/env python3

import copy
import functools
from xml.etree import ElementTree as ET

from PyQt5 import QtCore, QtGui, QtWidgets, QtSvg


class DndState:
    def __init__(self, view):
        self._view = view

    def view(self):
        return self._view

    def mousePressEvent(self, event): # pylint: disable=W0613
        return self

    def mouseReleaseEvent(self, event): # pylint: disable=W0613
        return self

    def mouseMoveEvent(self, event): # pylint: disable=W0613
        return self


class DndIdleState(DndState):
    def mousePressEvent(self, event):
        pixel = self.view().hitTest(event.pos())
        if pixel + 256 in self.view().pads():
            if event.button() == QtCore.Qt.LeftButton:
                return DndStartDraggingPadState(self.view(), event.pos(), pixel)
            if event.button() == QtCore.Qt.RightButton:
                self.view().setValue(pixel + 256, QtCore.QPointF(0, 0))
        elif event.button() == QtCore.Qt.LeftButton:
            if pixel in self.view().buttons():
                self.view().setValue(pixel, not self.view().value(pixel))
        return super().mousePressEvent(event)


class DndStartDraggingPadState(DndState):
    def __init__(self, view, initial_pos, index):
        super().__init__(view)
        self._initialPos = initial_pos
        self._index = index

    def mouseMoveEvent(self, event):
        if (event.pos() - self._initialPos).manhattanLength() >= QtWidgets.QApplication.startDragDistance():
            return DndDraggingPadState(self.view(), self._initialPos, event.pos(), self._index)
        return super().mouseMoveEvent(event)

    def mouseReleaseEvent(self, event):
        self.view().setValue(self._index, not self.view().value(self._index))
        return DndIdleState(self.view())


class DndDraggingPadState(DndState):
    def __init__(self, view, initial_pos, current_pos, index):
        super().__init__(view)
        self._initialPos = initial_pos
        self._currentPos = current_pos
        self._index = index
        self._delta = view.value(index + 256)

    def mouseMoveEvent(self, event):
        self._currentPos = event.pos()
        delta = (self.view().mapToSvg(self._currentPos) - self.view().mapToSvg(self._initialPos)) + self._delta
        self.view().setValue(self._index + 256, delta)
        self.view().setStatus(self._index)
        return super().mouseMoveEvent(event)

    def mouseReleaseEvent(self, event):
        self.view().clearStatus()
        self.view().updateCoords()
        return DndIdleState(self.view())


class DualShock4(QtWidgets.QWidget):
    INDEX_RPAD = 1
    INDEX_RPAD_TR = 257
    INDEX_LPAD = 2
    INDEX_LPAD_TR = 258
    INDEX_PS = 3
    INDEX_SHARE = 4
    INDEX_OPTIONS = 5
    INDEX_TRIANGLE = 6
    INDEX_SQUARE = 7
    INDEX_CIRCLE = 8
    INDEX_CROSS = 9
    INDEX_DPADU = 10
    INDEX_DPADD = 11
    INDEX_DPADL = 12
    INDEX_DPADR = 13
    INDEX_TPAD = 14
    INDEX_L1 = 15
    INDEX_R1 = 16
    INDEX_L2 = 17
    INDEX_R2 = 18

    def __init__(self, parent):
        super().__init__(parent)
        with open('res/images/dualshock.svg', 'r') as fileobj:
            self._svg = ET.parse(fileobj)

        self._state = DndIdleState(self)
        self._values = {}
        for pad in self.pads():
            self._values[pad] = QtCore.QPointF(0, 0)
        for btn in self.buttons():
            self._values[btn] = False
        self._values[self.INDEX_L2] = self._values[self.INDEX_R2] = 0
        self._status = ''

        self._l2 = QtWidgets.QSlider(QtCore.Qt.Vertical, self)
        self._l2.valueChanged.connect(functools.partial(self._valueChanged, self.INDEX_L2))
        self._l2.setMinimum(0)
        self._l2.setMaximum(255)
        self._r2 = QtWidgets.QSlider(QtCore.Qt.Vertical, self)
        self._r2.valueChanged.connect(functools.partial(self._valueChanged, self.INDEX_R2))
        self._r2.setMinimum(0)
        self._r2.setMaximum(255)

        self.updateCoords()

    def _valueChanged(self, index, value):
        self.setValue(index, value)

    def pads(self):
        return (
            self.INDEX_RPAD_TR,
            self.INDEX_LPAD_TR,
            )

    def buttons(self):
        return (
            self.INDEX_RPAD,
            self.INDEX_LPAD,
            self.INDEX_PS,
            self.INDEX_SHARE,
            self.INDEX_OPTIONS,
            self.INDEX_TRIANGLE,
            self.INDEX_SQUARE,
            self.INDEX_CIRCLE,
            self.INDEX_CROSS,
            self.INDEX_DPADU,
            self.INDEX_DPADD,
            self.INDEX_DPADL,
            self.INDEX_DPADR,
            self.INDEX_TPAD,
            self.INDEX_L1,
            self.INDEX_R1,
            )

    def indexToId(self, index):
        return {
            self.INDEX_RPAD: 'RPAD',
            self.INDEX_LPAD: 'LPAD',
            self.INDEX_PS: 'PS',
            self.INDEX_SHARE: 'SHARE',
            self.INDEX_OPTIONS: 'OPTIONS',
            self.INDEX_TRIANGLE: 'TRIANGLE',
            self.INDEX_SQUARE: 'SQUARE',
            self.INDEX_CIRCLE: 'CIRCLE',
            self.INDEX_CROSS: 'CROSS',
            self.INDEX_DPADU: 'DPADU',
            self.INDEX_DPADD: 'DPADD',
            self.INDEX_DPADL: 'DPADL',
            self.INDEX_DPADR: 'DPADR',
            self.INDEX_TPAD: 'TPAD',
            self.INDEX_RPAD_TR: 'RPAD',
            self.INDEX_LPAD_TR: 'LPAD',
            self.INDEX_L1: 'L1',
            self.INDEX_R1: 'R1',
            }[index]

    def value(self, index):
        return self._values[index]

    def setValue(self, index, value):
        if value:
            if index == self.INDEX_DPADU and self._values[self.INDEX_DPADD]:
                self._values[self.INDEX_DPADD] = False
            elif index == self.INDEX_DPADD and self._values[self.INDEX_DPADU]:
                self._values[self.INDEX_DPADU] = False
            elif index == self.INDEX_DPADL and self._values[self.INDEX_DPADR]:
                self._values[self.INDEX_DPADR] = False
            elif index == self.INDEX_DPADR and self._values[self.INDEX_DPADL]:
                self._values[self.INDEX_DPADL] = False

        if index in self.pads():
            line = QtCore.QLineF(QtCore.QPointF(0, 0), value)
            if line.length() > 16:
                line = QtCore.QLineF.fromPolar(16, line.angle())
            self._values[index] = QtCore.QPointF(line.dx(), line.dy())
        else:
            self._values[index] = value
        self.update()

    def setStatus(self, index):
        if index in self.pads():
            pt = self._values[index]
            dx = min(255, (pt.x() + 16) * 8)
            dy = min(255, (pt.y() + 16) * 8)
            self._status = 'X: %d; Y: %d' % (dx, dy)
        self.update()

    def clearStatus(self):
        self._status = ''
        self.update()

    def hitTest(self, pt):
        dx = (self.width() - self._hit.width()) / 2
        dy = self.height() - self._hit.height()
        pt = (pt - QtCore.QPointF(dx, dy)).toPoint()
        if 0 <= pt.x() < self._hit.width() and 0 <= pt.y() < self._hit.height():
            return self._hit.pixel(pt) & 0xFFFFFF
        return None

    def mapToSvg(self, pt):
        dx = (self.width() - self._hit.width()) / 2
        dy = self.height() - self._hit.height()
        return (QtCore.QPointF(pt) - QtCore.QPointF(dx, dy)) * 600 / self.width()

    def _fillElement(self, element, color, recurse=True):
        if 'style' in element.attrib:
            parts = element.attrib['style'].split(';')
            for index, part in enumerate(parts):
                if part.startswith('fill:'):
                    parts[index] = 'fill:#%06x' % color
                    break
            else:
                parts.append('fill:#%06x' % color)
            element.attrib['style'] = ';'.join(parts)
        else:
            element.attrib['style'] = 'fill:#%06x' % color

        if recurse:
            for child in element:
                self._fillElement(child, color)

    def paintEvent(self, event): # pylint: disable=W0613
        xml = copy.deepcopy(self._svg)
        for pad in self.pads():
            xml.find('.//*[@id="%s"]' % self.indexToId(pad)).attrib['transform'] = 'translate(%f %f)' % (self._values[pad].x(), self._values[pad].y())
        for btn in self.buttons():
            self._fillElement(xml.find('.//*[@id="%s"]' % self.indexToId(btn)), (0xFF0000 if self._values[btn] else 0xcccccc), recurse=btn not in {self.INDEX_PS, self.INDEX_TPAD})

        img = QtGui.QImage(self._hit.width(), self._hit.height(), QtGui.QImage.Format_RGB888)
        painter = QtGui.QPainter(img)
        painter.setBackground(QtCore.Qt.white)
        painter.eraseRect(self.rect())
        renderer = QtSvg.QSvgRenderer(ET.tostring(xml.getroot()))
        renderer.render(painter)

        painter = QtGui.QPainter(self)
        painter.drawImage(QtCore.QPoint((self.width() - self._hit.width()) // 2, self.height() - self._hit.height()), img)

        if self._status:
            mt = painter.fontMetrics()
            rc = QtCore.QRectF(0, self.height() - mt.height() - 1, self.width(), mt.height())
            painter.drawText(rc, QtCore.Qt.AlignCenter, self._status)

    def updateCoords(self):
        if self.width() / self.height() > 600 / 400:
            img = QtGui.QImage(600 * self.height() // 400, self.height(), QtGui.QImage.Format_RGB888)
        else:
            img = QtGui.QImage(self.width(), 400 * self.width() // 600, QtGui.QImage.Format_RGB888)

        xml = copy.deepcopy(self._svg)
        for index in self.buttons():
            self._fillElement(xml.find('.//*[@id="%s"]' % self.indexToId(index)), index)
        for index in self.pads():
            xml.find('.//*[@id="%s"]' % self.indexToId(index)).attrib['transform'] = 'translate(%f %f)' % (self._values[index].x(), self._values[index].y())

        painter = QtGui.QPainter(img)
        painter.setBackground(QtCore.Qt.white)
        painter.eraseRect(self.rect())
        renderer = QtSvg.QSvgRenderer(ET.tostring(xml.getroot()))
        renderer.render(painter)
        self._hit = img

    def resizeEvent(self, event):
        dx = (self.width() - self._hit.width()) // 2
        dy = self.height() - self._hit.height()
        self._l2.setGeometry(dx, dy, self._l2.sizeHint().width(), self._hit.height() // 4)
        self._r2.setGeometry(self.width() - dx - self._r2.sizeHint().width(), dy, self._r2.sizeHint().width(), self._hit.height() // 4)
        self.updateCoords()
        super().resizeEvent(event)

    def mousePressEvent(self, event):
        self._state = self._state.mousePressEvent(event)

    def mouseReleaseEvent(self, event):
        self._state = self._state.mouseReleaseEvent(event)

    def mouseMoveEvent(self, event):
        self._state = self._state.mouseMoveEvent(event)


if __name__ == '__main__':
    app = QtWidgets.QApplication([])
    win = DualShock4(None)
    win.show()
    app.exec_()
