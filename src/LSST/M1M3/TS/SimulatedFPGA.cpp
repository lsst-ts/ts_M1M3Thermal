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
#include "TSPublisher.h"

#include <cRIO/ModbusBuffer.h>
#include <cRIO/Timestamp.h>

#include <cstring>
#include <spdlog/spdlog.h>

using namespace LSST::cRIO;
using namespace LSST::M1M3::TS;

SimulatedFPGA::SimulatedFPGA() : IFPGA(), _U16ResponseStatus(IDLE) {
    response.simulateResponse(true);
    srandom(time(NULL));
}

void SimulatedFPGA::writeCommandFIFO(uint16_t* data, size_t length, uint32_t timeout) {
    uint16_t* d = data;
    while (d < data + length) {
        size_t dl;
        switch (*d) {
            case FPGAAddress::MODBUS_A_TX:
                d++;
                dl = *d;
                d++;
                _simulateModbus(d, dl);
                d += dl;
                break;
            case FPGAAddress::HEARTBEAT:
                d += 2;
                break;
            // modbus software trigger
            case 252:
                d++;
                break;
            default:
                SPDLOG_WARN(
                        "SimulatedFPGA::writeCommandFIFO unknown/unimplemented instruction: {0:04x} ({0:d})",
                        *d);
                d++;
                break;
        }
    }
}

void SimulatedFPGA::writeRequestFIFO(uint16_t* data, size_t length, uint32_t timeout) {
    _U16ResponseStatus = LEN;
}

void SimulatedFPGA::readU16ResponseFIFO(uint16_t* data, size_t length, uint32_t timeout) {
    switch (_U16ResponseStatus) {
        case IDLE:
            break;
        case LEN:
            *data = response.getLength();
            _U16ResponseStatus = DATA;
            break;
        case DATA:
            memcpy(data, response.getBuffer(), response.getLength() * 2);
            response.clear();
            _U16ResponseStatus = IDLE;
            break;
    }
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
                                         uint8_t fanRPM, float absoluteTemperature) {
    response.write<uint8_t>(address);
    switch (status & 0xF0) {
        case 0x10:
            response.write<uint8_t>(88);
            break;
        case 0x20:
            response.write<uint8_t>(89);
            break;
        default:
            throw std::runtime_error("Invalid status in SimulatedFPGA::processThermalStatus");
    }

    status |= getBroadcastCounter() << 4;

    response.write<uint8_t>(status);
    response.write<float>(differentialTemperature);
    response.write<uint8_t>(fanRPM);
    response.write<float>(absoluteTemperature);

    response.writeCRC();
}

void SimulatedFPGA::_simulateModbus(uint16_t* data, size_t length) {
    // reply format:
    // 4 bytes (forming uint64_t in low endian) beginning timestamp
    // data received from ILCs (& FIFO::TX_WAIT_LONG_RX)
    // end of frame (FIFO::RX_ENDFRAME)
    // 8 bytes of end timestap (& FIFO::RX_TIMESTAMP)

    response.writeFPGATimestamp(Timestamp::toFPGA(TSPublisher::getTimestamp()));

    ModbusBuffer buf(data, length);
    while (!buf.endOfBuffer()) {
        uint16_t p = buf.peek();
        if ((p & FIFO::CMD_MASK) != FIFO::WRITE) {
            buf.next();
            continue;
        }

        uint8_t address = buf.read<uint8_t>();
        uint8_t func = buf.read<uint8_t>();
        buf.checkCRC();
        switch (func) {
            // info
            case 17:
                // generate response
                processServerID(address, 0x01020304 + address, 0x02, 0x02, 0x02, 0x00, 1, 2,
                                "Test Thermal ILC");
                break;
            case 88:
                processThermalStatus(address, 0x10 | 0, 15.0 * random() / float(RAND_MAX),
                                     10 * random() / float(RAND_MAX), 20 * random() / float(RAND_MAX));
                break;
            case 89:
                processThermalStatus(address, 0x20 | 0, 15.0 * random() / float(RAND_MAX),
                                     10 * random() / float(RAND_MAX), 20 * random() / float(RAND_MAX));
                break;
            default:
                SPDLOG_WARN("SimulatedFPGA::_simulateModbus unknown/unsupported function {0:04x} ({0:d})",
                            func);
        }
        response.writeRxTimestamp(Timestamp::toFPGA(TSPublisher::getTimestamp()));

        response.writeRxEndFrame();
    }
}
