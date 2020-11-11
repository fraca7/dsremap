#!/usr/bin/env python3

import os
import sys
import asyncio
import subprocess
import codecs
import json
import signal
import logging
import getopt

import daemon
import daemon.pidfile
from aiohttp import web

VERSION = '1.0.0'
API_LEVEL = 1

# Ugly hack, but iterating over ALL possible FDs takes up to 70s without this, and about 12ms with it.
# See https://pagure.io/python-daemon/issue/40
def monkey_patch_daemon(exclude):
    return {int(name) for name in os.listdir('/proc/self/fd') if name.isdigit()}.difference(exclude)
import daemon.daemon
daemon.daemon._get_candidate_file_descriptors = monkey_patch_daemon


class DSRemapServer:
    logger = logging.getLogger('dsremap.server')

    def __init__(self):
        self.app = web.Application()
        self.app.add_routes([
            web.get('/info', self.handle_info),
            web.post('/set_config', self.handle_configuration),
            web.get('/setup_ds4', self.handle_setup_ds4),
            web.get('/setup_ps4', self.handle_setup_ps4),
            web.get('/reboot', self.handle_reboot),
            web.get('/halt', self.handle_halt),
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
        proc = await asyncio.create_subprocess_shell('service bluetooth stop')
        await proc.communicate()
        if proc.returncode:
            self.logger.warning('Cannot stop bluetooth service')

        await self.start_proxy()

    async def on_shutdown(self, app):
        await self.stop_proxy()

    async def start_proxy(self):
        info = self.current_pairing()
        if 'ds4' not in info or 'ps4' not in info:
            return

        # I have no idea what the BT service does but it seems to do something.
        proc = await asyncio.create_subprocess_shell('service bluetooth start')
        await proc.communicate()
        if proc.returncode:
            self.logger.warning('Cannot start bluetooth service')
        proc = await asyncio.create_subprocess_shell('service bluetooth stop')
        await proc.communicate()
        if proc.returncode:
            self.logger.warning('Cannot stop bluetooth service')
        proc = await asyncio.create_subprocess_shell('hciconfig hci0 up pscan')
        await proc.communicate()
        if proc.returncode:
            self.logger.warning('Cannot reconfigure interface')

        args = ['-h', info['ps4']['addr'], '-d', info['ds4']['addr'], '-s', info['ps4']['dongle']]
        if os.path.exists('/opt/dsremap/config.bin'):
            args.extend(['-b', '/opt/dsremap/config.bin'])

        self.logger.info('Starting proxy')
        self.proxy = await asyncio.create_subprocess_exec('/opt/dsremap/proxy', *args)

    async def stop_proxy(self):
        if self.proxy is None:
            return
        self.logger.info('Stopping proxy')
        self.proxy.send_signal(signal.SIGINT)
        await self.proxy.wait()
        self.logger.info('Proxy stopped')
        self.proxy = None

    async def handle_info(self, request):
        info = {'version': VERSION, 'api_level': API_LEVEL, 'type': 'rpi0w'} # May be more later
        info['bt_interfaces'] = []
        for name in os.listdir('/var/lib/bluetooth'):
            info['bt_interfaces'].append(name)
        info['current_pairing'] = self.current_pairing() or None

        return web.json_response(info)

    async def handle_configuration(self, request):
        src = '/opt/dsremap/config.part'
        if os.path.exists(src):
            os.remove(src)

        with open(src, 'wb') as fileobj:
            data = await request.content.read(4096)
            while data:
                fileobj.write(data)
                data = await request.content.read(4096)

        dst = '/opt/dsremap/config.bin'
        if os.path.exists(dst):
            os.remove(dst)
        os.rename(src, dst)

        await self.stop_proxy()
        await self.start_proxy()

        return web.json_response({'status': 'ok'})

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

    async def handle_reboot(self, request):
        proc = await asyncio.create_subprocess_shell('reboot')
        await proc.communicate()
        return web.json_response({'status': 'ok'})

    async def handle_halt(self, request):
        proc = await asyncio.create_subprocess_shell('halt -p')
        await proc.communicate()
        return web.json_response({'status': 'ok'})


def main():
    server = DSRemapServer()
    server.run()


if __name__ == '__main__':
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'd', ['daemon'])
    except getopt.GetoptError as exc:
        print(exc)
        sys.exit(1)

    detach = False

    for opt, val in opts:
        if opt in ('-d', '--daemon'):
            detach = True

    logopt = dict(level=logging.INFO, format='%(asctime)-15s %(levelname)-8s %(name)-15s %(message)s')
    if detach:
        with daemon.DaemonContext(pidfile=daemon.pidfile.PIDLockFile('/var/run/dsremap.pid')):
            logging.basicConfig(**logopt, filename='/var/log/dsremap.log')
            main()
    else:
        logging.basicConfig(**logopt)
        main()
