#!/usr/bin/env python
#
#  This file is part of libsamsung-ipc.
#
#  Copyright (C) 2020 Denis 'GNUtoo' Carikli <GNUtoo@cyberdimension.org>
#
#  libsamsung-ipc is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 2 of the License, or
#  (at your option) any later version.
#
#  libsamsung-ipc is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with libsamsung-ipc.  If not, see <http://www.gnu.org/licenses/>.

import os
import re
import sys
import sh

def usage(progname):
    print('{} [test]'.format(progname))
    sys.exit(1)

def get_output(data):
    return str(data).replace(os.linesep, '')

class IpcModem(object):
    def __init__(self):
        srcdir = os.environ.get('srcdir', None)

        command_path = ''
        if srcdir:
            command_path = '.' + os.sep + 'ipc-modem'
        # Enable to run tests without automake
        else:
            command_path = os.path.dirname(sys.argv[0]) \
                + os.sep \
                + 'ipc-modem'

        ipc_modem = sh.Command(command_path)
        self.ipc_modem = ipc_modem.bake('--dry-run')

    def test_help(self):
        try:
            self.ipc_modem()
        except sh.ErrorReturnCode_1:
            pass
        else:
            raise Exception()

    def test_boot(self, timeout=2):
        self.ipc_modem('boot',  _timeout=timeout)

    def test_power_on(self, timeout=2):
        self.ipc_modem('power-on',  _timeout=timeout)

    def test_power_off(self, timeout=2):
        self.ipc_modem('power-off', _timeout=timeout)

    def test_start(self, timeout=3):
        try:
            self.ipc_modem('start',  _timeout=timeout)
        except sh.TimeoutException:
            pass
        else:
            raise Exception()

    def test_call(self, timeout=3):
        expected_output = "[I] Got call number!"
        output = ""
        try:
            output = get_output(self.ipc_modem('power-on',
                                               '--call=0102030405',
                                               _timeout=timeout))
        except:
            raise Exception()

        if output != expected_output:
            raise Exception()

    def test_commands(self):
        self.test_boot()
        self.test_power_on()
        self.test_power_off()
        self.test_start()
        self.test_call()

def main():
    ipc_modem = IpcModem()
    ipc_modem.test_help()
    ipc_modem.test_commands()

if __name__ == '__main__':
    rc = main()
    sys.exit(rc)
