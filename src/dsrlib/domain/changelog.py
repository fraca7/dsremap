#!/usr/bin/env python3

import re
import io


class Change:
    def __init__(self, text):
        self.text = text


class Fix(Change):
    pass


class Feature(Change):
    pass


class Comment(Change):
    pass


class Version:
    def __init__(self, version):
        self._changes = []
        self._version = version

    def addChange(self, change):
        self._changes.append(change)

    def changes(self):
        return self._changes

    def version(self):
        return self._version


class Changelog:
    rx_version = re.compile(r'v(\d+)\.(\d+)\.(\d+)')

    def __init__(self, text):
        self._versions = []
        current = None
        for line in text.split('\n'):
            line = line.strip()

            match = self.rx_version.match(line)
            if match:
                version = (int(match.group(1)), int(match.group(2)), int(match.group(3)))
                current = Version(version)
                self._versions.append(current)
                continue
            if current is None:
                continue

            if line.startswith('#'):
                current.addChange(Comment(line[1:].strip()))
            if line.startswith('!'):
                current.addChange(Fix(line[1:].strip()))
            if line.startswith('+'):
                current.addChange(Feature(line[1:].strip()))

    def changesSince(self, current):
        result = []
        for version in self._versions:
            if version.version() > current:
                result.extend(version.changes())
        return result


class HTMLChangelogFormatter:
    def format(self, changes):
        buf = io.StringIO()
        self._format(changes, buf, _('New features'), Feature)
        self._format(changes, buf, _('Bugs fixes'), Fix)
        self._format(changes, buf, _('Others'), Comment)
        return buf.getvalue()

    def _format(self, changes, buf, label, cls):
        items = [change for change in changes if isinstance(change, cls)]
        if items:
            buf.write('<h1>%s</h1>' % label)
            buf.write('<ul>')
            for item in items:
                buf.write('<li>%s</li>' % item.text)
            buf.write('</ul>')
