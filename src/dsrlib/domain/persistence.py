#!/usr/bin/env python3

import os
import json
import uuid

from dsrlib.meta import Meta

from .actions import Axis, ActionVisitor, InvertPadAxisAction, SwapAxisAction, \
     GyroAction, CustomAction, DisableButtonAction
from .buttons import Buttons
from .configuration import Configuration


class BaseJSONWriter(ActionVisitor):
    VERSION = 0

    def encodeWorkspace(self, workspace):
        configurations = []
        for configuration in workspace.configurations():
            configurations.append(self.encodeConfiguration(configuration))
        return {'configurations': configurations}

    def encodeConfiguration(self, configuration):
        actions = []
        for action in configuration.actions():
            actions.append(self.encodeAction(action))
        return {'name': configuration.name(), 'uuid': configuration.uuid(), 'thumbnail': self.pathFor(configuration.thumbnail()), 'enabled': configuration.enabled(), 'actions': actions}

    def encodeAction(self, action):
        return self.visit(action)

    def _acceptDisableButtonAction(self, action):
        return {'type': 'disable_button', 'button': action.button().name}

    def _acceptInvertPadAxisAction(self, action):
        return {'type': 'invert_pad', 'pad': action.pad(), 'axis': action.axis()}

    def _acceptSwapAxisAction(self, action):
        axis1, axis2 = action.axis()
        return {'type': 'swap_pads', 'axis1': axis1.name, 'axis2': axis2.name}

    def _acceptGyroAction(self, action):
        return {'type': 'gyro', 'buttons': [button.name for button in action.buttons()]}

    def _acceptCustomAction(self, action):
        return {'type': 'custom', 'code': action.source()}

    def pathFor(self, filename):
        raise NotImplementedError


class JSONWriter(BaseJSONWriter):
    def write(self, stream, workspace):
        data = self.encodeWorkspace(workspace)
        json.dump({'version': self.VERSION, 'workspace': data}, stream)

    def pathFor(self, filename):
        return filename


class JSONExporter(BaseJSONWriter):
    def __init__(self, zipobj):
        self._zipobj = zipobj
        self._count = 0

    def write(self, configuration):
        data = self.encodeConfiguration(configuration)
        data['enabled'] = False
        data['version'] = self.VERSION
        data['uuid'] = uuid.uuid1().hex
        self._zipobj.writestr('configuration.json', json.dumps(data, indent=2))

    def pathFor(self, filename):
        _, ext = os.path.splitext(filename)
        name = '%d%s' % (self._count, ext)
        self._count += 1
        self._zipobj.write(filename, name)
        return name


class BaseJSONReader:
    def decodeWorkspace(self, workspace, data):
        self.version = data['version'] # pylint: disable=W0201
        for cdata in data['workspace']['configurations']:
            configuration = self.decodeConfiguration(cdata)
            workspace.configurations().addItem(configuration)

    def decodeConfiguration(self, data):
        configuration = Configuration(uid=data['uuid'])
        configuration.setName(data['name'])
        configuration.setThumbnail(self.pathFor(data['thumbnail']))
        configuration.setEnabled(data['enabled'])

        for adata in data['actions']:
            action = self.decodeAction(adata)
            configuration.addAction(action)
        return configuration

    def decodeAction(self, data):
        if data['type'] == 'disable_button':
            action = DisableButtonAction()
            action.setButton(Buttons[data['button']])
        if data['type'] == 'invert_pad':
            action = InvertPadAxisAction()
            action.setPad(data['pad'])
            action.setAxis(data['axis'])
        if data['type'] == 'swap_pads':
            action = SwapAxisAction()
            action.setAxis1(Axis[data['axis1']])
            action.setAxis2(Axis[data['axis2']])
        if data['type'] == 'gyro':
            action = GyroAction()
            action.setButtons([Buttons[name] for name in data['buttons']])
        if data['type'] == 'custom':
            action = CustomAction()
            action.setSource(data['code'])
        return action

    def pathFor(self, filename):
        raise NotImplementedError


class JSONReader(BaseJSONReader):
    def read(self, stream, workspace):
        data = json.load(stream)
        self.version = data['version'] # pylint: disable=W0201
        self.decodeWorkspace(workspace, data)

    def pathFor(self, filename):
        return filename


class JSONImporter(BaseJSONReader):
    def __init__(self):
        self._zipobj = None

    def read(self, zipobj):
        self._zipobj = zipobj
        data = json.loads(zipobj.read('configuration.json').decode('utf-8'))
        return self.decodeConfiguration(data)

    def pathFor(self, filename):
        dst = Meta.newThumbnail(filename)
        self._zipobj.extract(filename, os.path.dirname(dst))
        os.rename(os.path.join(os.path.dirname(dst), filename), dst)
        return dst
