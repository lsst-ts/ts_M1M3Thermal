# This file is part of M1M3TS.
#
# Developed for the Rubin Observatory Telescope and Site System.
# This product includes software developed by the LSST Project
# (https://www.lsst.org).
# See the COPYRIGHT file at the top-level directory of this distribution
# for details of code ownership.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import asyncio
import unittest

from lsst.ts.salobj import Domain, Remote, State


class CSCTestCase(unittest.IsolatedAsyncioTestCase):
    async def test_commands(self) -> None:
        async with Domain() as domain, Remote(domain=domain, name="MTM1M3TS") as m1m3ts:
            await m1m3ts.cmd_start.start()
            await asyncio.sleep(10)
            await m1m3ts.cmd_enable.start()
            assert m1m3ts.evt_summaryState.get().summaryState == State.ENABLED
