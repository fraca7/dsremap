#!/usr/bin/env python3

import os
import asyncio
import subprocess
import codecs
import json

from aiohttp import web


class DSRemapServer:
    def __init__(self):
        self.app = web.Application()
        self.app.add_routes([web.get('/pair', self._handle_pairing)])

    def run(self):
        web.run_app(self.app)

    async def _handle_pairing(self, request):
        # We assume we have exactly one BT interface.
        dongle = os.listdir('/var/lib/bluetooth')[0]
        ds4 = request.query['ds4_btaddr']

        proc = await asyncio.create_subprocess_exec('/opt/dsremap/pairing', '-d', dongle, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = await proc.communicate()
        if proc.returncode:
            return web.json_response({'status': 'ko', 'code': proc.returncode, 'stderr': stderr.decode('ascii')})

        for line in stdout.decode('ascii').split('\n'):
            if line.startswith('!! PS4: '):
                ps4 = line[8:].strip()
            if line.startswith('!! KEY: '):
                key = line[8:].strip()

        # Setup stuff
        self._write_linkkey(dongle, ds4, key)
        self._write_linkkey(dongle, ps4, key)

        with codecs.getwriter('utf-8')(open('/opt/dsremap/current_pairing.json', 'wb')) as fileobj:
            json.dump({'ps4': ps4, 'ds4': ds4}, fileobj, indent=2)

        return web.json_response({'status': 'ok', 'ps4': ps4, 'key': key})

    def _write_linkkey(self, dongle, btaddr, key):
        path = '/var/lib/bluetooth/%s/%s' % (dongle, btaddr)
        if not os.path.exists(path):
            os.mkdir(path)
        with codecs.getwriter('utf-8')(open(os.path.join(path, 'info'), 'wb')) as fileobj:
            fileobj.write('[LinkKey]\n')
            fileobj.write('Key=%s\n' % key)
            fileobj.write('PINLength=0\n')
            fileobj.write('Type=4\n')


def main():
    server = DSRemapServer()
    server.run()


if __name__ == '__main__':
    main()
