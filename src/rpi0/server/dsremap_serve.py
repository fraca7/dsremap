#!/usr/bin/env python3

import os
import asyncio
import subprocess
import codecs
import json
import signal
import logging

from aiohttp import web


class DSRemapServer:
    logger = logging.getLogger('dsremap.server')

    def __init__(self):
        self.app = web.Application()
        self.app.add_routes([
            web.get('/info', self.handle_info),
            web.get('/setup_ds4', self.handle_setup_ds4),
            web.get('/setup_ps4', self.handle_setup_ps4),
            ])
        self.proxy = None

        self.app.on_startup.append(self.on_startup)
        self.app.on_shutdown.append(self.on_shutdown)

    def run(self):
        web.run_app(self.app)

    def current_pairing(self):
        filename = '/opt/dsremap/current_pairing.json'
        if os.path.exists(filename):
            try:
                with codecs.getreader('utf-8')(open(filename, 'rb')) as fileobj:
                    return json.load(fileobj)
            except:
                pass
        return {}

    def set_current_pairing(self, data):
        with codecs.getwriter('utf-8')(open('/opt/dsremap/current_pairing.json', 'wb')) as fileobj:
            json.dump(data, fileobj)

    def write_linkkey(self, dongle, btaddr, key):
        path = '/var/lib/bluetooth/%s/%s' % (dongle, btaddr)
        if not os.path.exists(path):
            os.mkdir(path)
        with codecs.getwriter('utf-8')(open(os.path.join(path, 'info'), 'wb')) as fileobj:
            fileobj.write('[LinkKey]\n')
            fileobj.write('Key=%s\n' % key)
            fileobj.write('PINLength=0\n')
            fileobj.write('Type=4\n')

    async def on_startup(self, app):
        await self.start_proxy()

    async def on_shutdown(self, app):
        await self.stop_proxy()

    async def start_proxy(self):
        info = self.current_pairing()
        if 'ds4' not in info or 'ps4' not in info:
            return
        self.logger.info('Starting proxy')
        self.proxy = await asyncio.create_subprocess_exec('/opt/dsremap/proxy',
                                                          '-h', info['ps4']['addr'],
                                                          '-d', info['ds4']['addr'],
                                                          '-s', info['ps4']['dongle'])

    async def stop_proxy(self):
        if self.proxy is None:
            return
        self.logger.info('Stopping proxy')
        self.proxy.send_signal(signal.SIGINT)
        await self.proxy.wait()
        self.logger.info('Proxy stopped')
        self.proxy = None

    async def handle_info(self, request):
        info = {'type': 'rpi0w'} # May be more later
        info['bt_interfaces'] = []
        for name in os.listdir('/var/lib/bluetooth'):
            info['bt_interfaces'].append(name)
        info['current_pairing'] = self.current_pairing() or None

        return web.json_response(info)

    async def handle_setup_ds4(self, request):
        dongle = request.query['interface']
        ds4 = request.query['ds4']
        key = request.query['link_key']

        self.write_linkkey(dongle, ds4, key)

        info = self.current_pairing()
        info['ds4'] = {'dongle': dongle, 'addr': ds4, 'key': key}
        self.set_current_pairing(info)

        await self.stop_proxy()
        await self.start_proxy()

        return web.json_response({'status': 'ok', 'interface': dongle, 'ds4': ds4, 'link_key': key})

    async def handle_setup_ps4(self, request):
        dongle = request.query['interface']

        proc = await asyncio.create_subprocess_exec('/opt/dsremap/pairing', '-d', dongle, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = await proc.communicate()
        if proc.returncode:
            return web.json_response({'status': 'ko', 'code': proc.returncode, 'stderr': stderr.decode('ascii')})

        for line in stdout.decode('ascii').split('\n'):
            if line.startswith('!! PS4: '):
                ps4 = line[8:].strip()
            if line.startswith('!! KEY: '):
                key = line[8:].strip()

        self.write_linkkey(dongle, ps4, key)

        info = self.current_pairing()
        info['ps4'] = {'dongle': dongle, 'addr': ps4, 'key': key}
        self.set_current_pairing(info)

        await self.stop_proxy()
        await self.start_proxy()

        return web.json_response({'status': 'ok', 'ps4': ps4, 'key': key})


def main():
    server = DSRemapServer()
    server.run()


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO, format='%(asctime)-15s %(levelname)-8s %(name)-15s %(message)s')
    main()
