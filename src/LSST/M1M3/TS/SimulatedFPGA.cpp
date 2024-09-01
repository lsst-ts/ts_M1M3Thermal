/*
 * Simulated thermal FPGA class.
 *
 * Developed for the Vera C. Rubin Observatory Telescope & Site Software
 * Systems. This product includes software developed by the Vera C.Rubin
 * Observatory Project (https://www.lsst.org). See the COPYRIGHT file at the
 * top-level directory of this distribution for details of code ownership.
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

#include <cstring>
#include <spdlog/spdlog.h>

#include <cRIO/ModbusBuffer.h>
#include <cRIO/Timestamp.h>

#include "SimulatedFPGA.h"
#include "TSPublisher.h"

using namespace LSST::cRIO;
using namespace LSST::M1M3::TS;

SimulatedFPGA::SimulatedFPGA() : ILC::ILCBusList(1), IFPGA(), ThermalILC(1), _U16ResponseStatus(IDLE) {
    _broadcastCounter = 0;
    srandom(time(NULL));
    for (int i = 0; i < NUM_TS_ILC; i++) {
        _mode[i] = ILC::Mode::Standby;
        _heaterPWM[i] = 0;
        _fanRPM[i] = 0;
    }

    _flowmeter_net_totalizer = 100.0;
    _pump_voltage = 1;
}

SimulatedFPGA::~SimulatedFPGA() {}

void SimulatedFPGA::writeMPUFIFO(MPU &mpu, const std::vector<uint8_t> &data, uint32_t timeout) {
    writeDebugFile<uint8_t>(fmt::format("MPU {} <", mpu.getBus()), data);

    auto write_answers = [this](uint8_t bus, const uint8_t *data, uint8_t len) {
        Modbus::Parser parser(std::vector<uint8_t>(data, data + len));
        Modbus::Buffer response;
        response.push_back(parser.address());
        switch (parser.func()) {
            case MPU::READ_HOLDING_REGISTERS: {
                uint16_t reg = parser.read<uint16_t>();
                uint16_t reg_len = parser.read<uint16_t>() * 2;
                response.push_back(parser.func());
                response.push_back(reg_len);
                for (size_t i = 0; i < reg_len; i += 2, reg++) {
                    // simulates various registers
                    switch (reg) {
                        case 2500:
                            response.write<float>(_flowmeter_net_totalizer);
                            break;
                        case 2501:
                            _flowmeter_net_totalizer += 11.1;
                            break;
                        case 2502:
                            response.write<float>(_flowmeter_net_totalizer + 50);
                            break;
                        case 2504:
                            response.write<float>(_flowmeter_net_totalizer - 50);
                            break;
                        case 2503:
                        case 2505:
                            break;
                        case 0x2106:
                            response.write<uint16_t>(_pump_voltage);
                            _pump_voltage += 1;
                            break;
                        default:
                            response.push_back(i);
                            response.push_back(i + 1);
                    }
                }
                break;
            }
            case MPU::PRESET_HOLDING_REGISTER: {
                uint16_t reg = parser.read<uint16_t>();
                uint16_t reg_len = parser.read<uint16_t>();

                response.push_back(parser.func());
                response.write(reg);
                response.write(reg_len);

                break;
            }
            default:
                throw std::runtime_error(
                        fmt::format("Response for function {} not implemented", parser.func()));
        }
        response.writeCRC();
        _mpuResponses[bus] = response;
    };

    write_answers(mpu.getBus(), data.data(), data.size());
}

std::vector<uint8_t> SimulatedFPGA::readMPUFIFO(cRIO::MPU &mpu) {
    auto ret = _mpuResponses[mpu.getBus()];
    _mpuResponses[mpu.getBus()].clear();
    return ret;
}

void SimulatedFPGA::writeCommandFIFO(uint16_t *data, size_t length, uint32_t timeout) {
    uint16_t *d = data;
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
            case FPGAAddress::GLYCOLTEMP_TEMPERATURES:
                d++;
                break;
            // modbus software trigger
            case 252:
                d++;
                break;
            default:
                SPDLOG_WARN(
                        "SimulatedFPGA::writeCommandFIFO unknown/unimplemented "
                        "instruction: {0:04x} ({0:d})",
                        *d);
                d++;
                break;
        }
    }
}

void SimulatedFPGA::writeRequestFIFO(uint16_t *data, size_t length, uint32_t timeout) {
    _U16ResponseStatus = LEN;
}

void SimulatedFPGA::readSGLResponseFIFO(float *data, size_t length, uint32_t timeout) {
    for (size_t i = 0; i < length; i++) {
        data[i] = i + random() / (float)RAND_MAX;
    }
}

void SimulatedFPGA::readU8ResponseFIFO(uint8_t *data, size_t length, uint32_t timeout) {
    for (size_t i = 0; i < length; i++) {
        data[i] = 255 * (random() / RAND_MAX);
    }
}

void SimulatedFPGA::readU16ResponseFIFO(uint16_t *data, size_t length, uint32_t timeout) {
    switch (_U16ResponseStatus) {
        case IDLE:
            break;
        case LEN:
            *data = _response.getLength();
            _U16ResponseStatus = DATA;
            break;
        case DATA:
            memcpy(data, _response.getBuffer(), _response.getLength() * 2);
            _response.clear();
            _U16ResponseStatus = IDLE;
            break;
    }
}

float SimulatedFPGA::chassisTemperature() { return -5 + 5.0f * random() / RAND_MAX; }

void SimulatedFPGA::processServerID(uint8_t address, uint64_t uniqueID, uint8_t ilcAppType,
                                    uint8_t networkNodeType, uint8_t ilcSelectedOptions,
                                    uint8_t networkNodeOptions, uint8_t majorRev, uint8_t minorRev,
                                    std::string firmwareName) {
    // construct _response in ILC
    _response.write<uint8_t>(address);
    _response.write<uint8_t>(17);
    _response.write<uint8_t>(12 + firmwareName.length());

    // uniqueID
    _response.writeBuffer(reinterpret_cast<uint8_t *>(&uniqueID), 6);

    _response.write<uint8_t>(ilcAppType);
    _response.write<uint8_t>(networkNodeType);
    _response.write<uint8_t>(ilcSelectedOptions);
    _response.write<uint8_t>(networkNodeOptions);
    _response.write<uint8_t>(majorRev);
    _response.write<uint8_t>(minorRev);

    for (auto c : firmwareName) {
        _response.write<uint8_t>(c);
    }

    _response.writeCRC();
}

void SimulatedFPGA::processServerStatus(uint8_t address, uint8_t mode, uint16_t status, uint16_t faults) {
    _response.write(address);
    _response.write<uint8_t>(18);
    _response.write(mode);
    _response.write(status);
    _response.write(faults);
    _response.writeCRC();
}

void SimulatedFPGA::processChangeILCMode(uint8_t address, uint16_t mode) {
    _mode[address - 1] = mode;
    _response.write(address);
    _response.write<uint8_t>(65);
    _response.write(mode);
    _response.writeCRC();
}

void SimulatedFPGA::processSetTempILCAddress(uint8_t address, uint8_t newAddress) {}

void SimulatedFPGA::processResetServer(uint8_t address) {}

void SimulatedFPGA::processThermalStatus(uint8_t address, uint8_t status, float differentialTemperature,
                                         uint8_t fanRPM, float absoluteTemperature) {
    _response.write<uint8_t>(address);
    switch (status & 0xF0) {
        case 0x10:
            _response.write<uint8_t>(88);
            break;
        case 0x20:
            _response.write<uint8_t>(89);
            break;
        default:
            throw std::runtime_error("Invalid status in SimulatedFPGA::processThermalStatus");
    }

    status |= getBroadcastCounter() << 4;

    _response.write<uint8_t>(status);
    _response.write<float>(differentialTemperature);
    _response.write<uint8_t>(fanRPM);
    _response.write<float>(absoluteTemperature);

    _response.writeCRC();
}

void SimulatedFPGA::processReHeaterGains(uint8_t address, float proportionalGain, float integralGain) {
    _response.write<uint8_t>(address);
    _response.write<uint8_t>(92);

    _response.writeCRC();
}

void SimulatedFPGA::processMPURead(uint8_t bus, uint8_t address, uint16_t register_address, uint16_t len) {
    static float simFlowMeter = 0;
    static uint16_t commandedFrequency = 0;

    auto response = &(_mpuResponses[bus]);
    response->write<uint8_t>(address);
    response->write<uint8_t>(3);
    response->write<uint8_t>(len * 2);

    union {
        float fval;
        uint16_t data[2];
    } float2bus;

    for (int i = 0; i < len; i++) {
        // for the moment VFD and FlowMeter registers are distinct, so we don't
        // care about distinguishing if the desired output is for VFD or
        // FlowMeter. The following code can be easily changed if that becomes
        // an issue, or more complex simulation is needed in future,
        switch (register_address + i) {
            case 1000:
            case 2500:
            case 2502:
            case 2504:
                simFlowMeter += 0.1 + 0.01 * random() / float(RAND_MAX);

                float2bus.fval = simFlowMeter;
                response->write<uint16_t>(float2bus.data[1]);
                response->write<uint16_t>(float2bus.data[0]);
                i++;
                break;
            case 5500:
                response->write<uint16_t>(1000 * random() / float(RAND_MAX));
                break;
            case 0x2001:
                response->write<uint16_t>(commandedFrequency * 100);
                commandedFrequency++;
                commandedFrequency %= 60;
                break;
            default:
                response->write<uint16_t>(register_address + i + 10 * (random() / float(RAND_MAX)));
        }
    }

    response->writeCRC();
}

void SimulatedFPGA::_simulateModbus(uint16_t *data, size_t len) {
    // reply format:
    // 4 bytes (forming uint64_t in low endian) beginning timestamp
    // data received from ILCs (& FIFO::TX_WAIT_LONG_RX)
    // end of frame (FIFO::RX_ENDFRAME)
    // 8 bytes of end timestap (& FIFO::RX_TIMESTAMP)

    _response.writeFPGATimestamp(Timestamp::toFPGA(TSPublisher::getTimestamp()));

    SimulatedILC buf(data, len);
    while (!buf.endOfBuffer()) {
        uint16_t p = buf.peek();
        if ((p & FIFO::CMD_MASK) != FIFO::WRITE) {
            buf.next();
            continue;
        }

        uint8_t address = buf.read<uint8_t>();
        uint8_t func = buf.read<uint8_t>();
        // broadcasts addresses
        if (address >= 248 && address <= 250) {
            _broadcastCounter = buf.read<uint8_t>();
            switch (func) {
                // Modbus functions - please see ILC protocol document for details
                case 88:
                    for (int i = 0; i < NUM_TS_ILC; i++) {
                        _heaterPWM[i] = buf.read<uint8_t>();
                        _fanRPM[i] = buf.read<uint8_t>();
                    }
                    break;
                default:
                    SPDLOG_WARN("Broadcast function {} is not being simulated", func);
            }
        } else {
            switch (func) {
                // Modbus functions - please see ILC protocol document for details
                case 17:
                    // generate _response
                    processServerID(address, 0x01020304 + address, 0x02, 0x02, 0x02, 0x00, 1, 2,
                                    "Test Thermal ILC");
                    break;
                case 18:
                    processServerStatus(address, _mode[address - 1], 0, 0);
                    break;
                case 65:
                    processChangeILCMode(address, buf.read<uint16_t>());
                    break;
                case 88:
                    _heaterPWM[address - 1] = buf.read<uint8_t>();
                    _fanRPM[address - 1] = buf.read<uint8_t>();
                    processThermalStatus(address, 0x10 | _broadcastCounter, 15.0 * random() / float(RAND_MAX),
                                         _fanRPM[address - 1], 20 * random() / float(RAND_MAX));
                    break;
                case 89:
                    processThermalStatus(address, 0x20 | _broadcastCounter, 15.0 * random() / float(RAND_MAX),
                                         _fanRPM[address - 1], 20 * random() / float(RAND_MAX));
                    break;
                default:
                    SPDLOG_WARN(
                            "SimulatedFPGA::_simulateModbus unknown/unsupported "
                            "function {0:04x} ({0:d})",
                            func);
            }

            _response.writeRxTimestamp(Timestamp::toFPGA(TSPublisher::getTimestamp()));

            _response.writeRxEndFrame();
        }
        buf.checkCRC();
    }
}

void SimulatedFPGA::_simulateMPU(uint8_t bus, uint8_t *data, size_t len) {
    Modbus::Parser buf(std::vector<uint8_t>(data, data + len));
    uint8_t address = buf.address();
    uint8_t func = buf.func();
    switch (func) {
        case 3: {
            uint16_t reg_add = buf.read<uint16_t>();
            uint16_t reg_len = buf.read<uint16_t>();
            processMPURead(bus, address, reg_add, reg_len);
            break;
        }
        default:
            SPDLOG_WARN(
                    "SimulatedFPGA::_simulateMPU unknow/unsupported function "
                    "{0:04x} ({0:d})",
                    func);
    }
    buf.checkCRC();
}
