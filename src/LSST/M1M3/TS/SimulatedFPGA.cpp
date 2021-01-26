/*
 * Simulated thermal FPGA class.
 *
 * Developed for the Vera C. Rubin Observatory Telescope & Site Software Systems.
 * This product includes software developed by the Vera C.Rubin Observatory Project
 * (https://www.lsst.org). See the COPYRIGHT file at the top-level directory of
 * this distribution for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "SimulatedFPGA.h"

#include <cRIO/ModbusBuffer.h>

#include <cstring>

namespace LSST {
namespace M1M3 {
namespace TS {

void SimulatedFPGA::writeCommandFIFO(uint16_t* data, size_t length, uint32_t timeout) {
    uint16_t* d = data;
    while (d < data + length) {
        size_t dl;
        switch (*d) {
            case 0x09:
                d++;
                dl = *d;
                d++;
                _simulateModbus(d, dl);
                d += dl;
                break;
        }
    }
}

void SimulatedFPGA::writeRequestFIFO(uint16_t* data, int32_t length, int32_t timeout) {}

void SimulatedFPGA::readU16ResponseFIFO(uint16_t* data, int32_t length, int32_t timeout) {
    memcpy(data, response.getBuffer(), response.getLength());
}

void SimulatedFPGA::processServerID(uint8_t address, uint64_t uniqueID, uint8_t ilcAppType,
                                    uint8_t networkNodeType, uint8_t ilcSelectedOptions,
                                    uint8_t networkNodeOptions, uint8_t majorRev, uint8_t minorRev,
                                    std::string firmwareName) {
    // construct response in ILC
    response.write<uint8_t>(address);
    response.write<uint8_t>(17);
    response.write<uint8_t>(12 + firmwareName.length());

    // uniqueID
    response.writeBuffer(reinterpret_cast<uint8_t*>(&uniqueID), 6);

    response.write<uint8_t>(ilcAppType);
    response.write<uint8_t>(networkNodeType);
    response.write<uint8_t>(ilcSelectedOptions);
    response.write<uint8_t>(networkNodeOptions);
    response.write<uint8_t>(majorRev);
    response.write<uint8_t>(minorRev);

    for (auto c : firmwareName) {
        response.write<uint8_t>(c);
    }

    response.writeCRC();
}

void SimulatedFPGA::processServerStatus(uint8_t address, uint8_t mode, uint16_t status, uint16_t faults) {}

void SimulatedFPGA::processChangeILCMode(uint8_t address, uint16_t mode) {}

void SimulatedFPGA::processSetTempILCAddress(uint8_t address, uint8_t newAddress) {}

void SimulatedFPGA::processResetServer(uint8_t address) {}

void SimulatedFPGA::processThermalStatus(uint8_t address, uint8_t status, float differentialTemperature,
                                         uint8_t fanRPM, float absoluteTemperature) {}

void SimulatedFPGA::_simulateModbus(uint16_t* data, size_t length) {
    ModbusBuffer buf(data, length);
    while (!buf.endOfBuffer()) {
        uint8_t address = buf.read<uint8_t>();
        uint8_t func = buf.read<uint8_t>();
        switch (func) {
            // info
            case 17:
                processServerID(address, 0x01020304 + address, 0x02, 0x02, 0x02, 0x00, 1, 2,
                                "Test Thermal ILC");
                break;
        }
    }
}

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST
