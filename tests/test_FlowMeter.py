#!/usr/bin/env python3
# This file is part of ts_m1m3thermal.
#
# Developed for the Vera Rubin Observatory Telescope and Site Systems.
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
# along with this program. If not, see <https://www.gnu.org/licenses/>.

import enum
import os
import unittest

import pymodbus.client as ModbusClient


class Registers(enum.IntEnum):
    FLOW_RATE = 1600
    VELOCITY = 1604
    NET_TOTALIZER = 2800
    POSITIVE_TOTALIZER = 2804
    NEGATIVE_TOTALIZER = 2808
    SIGNAL_STRENGTH = 5500
    TAG = 7000
    PART_NUMBER = 7064
    SERIAL_NUMBER = 7128
    FIRMWARE_VERSION = 7192
    CALIBRATION_DATE = 7256
    DATE_CODE = 7320


class FlowMeterTest(unittest.IsolatedAsyncioTestCase):
    HOST: str = "localhost"
    PORT: int = 1502

    async def asyncSetUp(self) -> None:
        print(f"Connecting to Modbus TCP/IP server on {FlowMeterTest.HOST}:{FlowMeterTest.PORT}.")
        self.client = ModbusClient.AsyncModbusTcpClient(self.HOST, port=self.PORT)

        await self.client.connect()
        assert self.client.connected, "Either Simulator is not running or you forgot to provide HOST address."

    async def test_coils(self) -> None:
        coils = await self.client.read_coils(0, count=10)
        assert coils.isError()
        assert coils.exception_code == 1

    async def test_registers(self) -> None:
        rr = await self.client.read_holding_registers(Registers.FLOW_RATE, count=16, device_id=1)
        assert not (rr.isError())
        flow_rate = self.client.convert_from_registers(
            rr.registers[0:2],
            data_type=self.client.DATATYPE.FLOAT32,
        )
        assert flow_rate > 1, f"Flow rate is {flow_rate}."


if __name__ == "__main__":
    FlowMeterTest.HOST = os.environ.get("HOST", FlowMeterTest.HOST)
    FlowMeterTest.PORT = int(os.environ.get("PORT", FlowMeterTest.PORT))
    unittest.main()
