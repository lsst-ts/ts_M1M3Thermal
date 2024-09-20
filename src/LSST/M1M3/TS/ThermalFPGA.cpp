/*
 * Thermal FPGA class.
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

#include <iostream>
#include <thread>

#include <spdlog/spdlog.h>

#include <cRIO/NiError.h>

#include "NiFpga_ts_M1M3ThermalFPGA.h"
#include "ThermalFPGA.h"

using namespace LSST::cRIO;
using namespace LSST::M1M3::TS;

using namespace std::chrono_literals;

ThermalFPGA::ThermalFPGA() : IFPGA() {
    SPDLOG_DEBUG("ThermalFPGA: ThermalFPGA()");
    _session = 0;
}

ThermalFPGA::~ThermalFPGA() {}

void ThermalFPGA::initialize() {
    SPDLOG_DEBUG("ThermalFPGA: initialize()");
    NiThrowError(__PRETTY_FUNCTION__, NiFpga_Initialize());
}

void ThermalFPGA::open() {
    SPDLOG_DEBUG("ThermalFPGA: open()");
    NiOpen("/var/lib/M1M3TS", NiFpga_ts_M1M3ThermalFPGA, "RIO0", 0, &(_session));
    NiThrowError(__PRETTY_FUNCTION__, "NiFpga_Abort", NiFpga_Abort(_session));
    NiThrowError(__PRETTY_FUNCTION__, "NiFpga_Download", NiFpga_Download(_session));
    NiThrowError(__PRETTY_FUNCTION__, "NiFpga_Reset", NiFpga_Reset(_session));
    NiThrowError(__PRETTY_FUNCTION__, "NiFpga_Run", NiFpga_Run(_session, 0));
}

void ThermalFPGA::close() {
    SPDLOG_DEBUG("ThermalFPGA: close()");
    for (auto c : _contexes) {
        NiFpga_UnreserveIrqContext(_session, c.second);
    }
    NiThrowError(__PRETTY_FUNCTION__, "NiFpga_Close", NiFpga_Close(_session, 0));
}

void ThermalFPGA::finalize() {
    SPDLOG_DEBUG("ThermalFPGA: finalize()");
    NiThrowError(__PRETTY_FUNCTION__, NiFpga_Finalize());
}

void ThermalFPGA::writeMPUFIFO(MPU &mpu, const std::vector<uint8_t> &data, uint32_t timeout) {
    writeDebugFile<uint8_t>(fmt::format("MPU {} <", mpu.getBus()), data);

    int32_t bus_id = 0;
    switch (mpu.getBus()) {
        case SerialBusses::GLYCOOL_BUS:
            bus_id = NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_GlycoolWrite;
            break;
        case SerialBusses::FLOWMETER_BUS:
            bus_id = NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_FlowMeterWrite;
            break;
        default:
            throw std::runtime_error(fmt::format("Invalid bus number - {}", mpu.getBus()));
    }

    uint8_t header[2] = {1, static_cast<uint8_t>(data.size())};
    NiThrowError(__PRETTY_FUNCTION__, NiFpga_WriteFifoU8(_session, bus_id, header, 2, timeout, NULL));

    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_WriteFifoU8(_session, bus_id, data.data(), data.size(), timeout, NULL));
}

std::vector<uint8_t> ThermalFPGA::readMPUFIFO(MPU &mpu) {
    uint32_t instr_bus = 0;
    uint32_t bus_id = 0;
    switch (mpu.getBus()) {
        case SerialBusses::GLYCOOL_BUS:
            instr_bus = NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_GlycoolWrite;
            bus_id = NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_GlycoolRead;
            break;
        case SerialBusses::FLOWMETER_BUS:
            instr_bus = NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_FlowMeterWrite;
            bus_id = NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_FlowMeterRead;
            break;
        default:
            throw std::runtime_error(fmt::format("Invalid bus number - {}", mpu.getBus()));
    }

    uint8_t data[255];
    size_t len = 0;

    int remaining_time = 2000;

    // bussy wait for data
    while (remaining_time > 0) {
        uint8_t req = 2;
        uint8_t response[2];

        NiThrowError("ThermalFPGA::readMPUFIFO: requesting response",
                     NiFpga_WriteFifoU8(_session, instr_bus, &req, 1, 0, NULL));

        NiThrowError("ThermalFPGA::readMPUFIFO: reading response and its length",
                     NiFpga_ReadFifoU8(_session, bus_id, response, 2, 10, NULL));

        if (response[0] != 2) {
            throw std::runtime_error(fmt::format("Invalid reply from bus - {}, expected 2", response[0]));
        }
        if (response[1] == 0) {
            // if there isn't reply and some data were received, process those - don't read further
            if (len > 0) {
                break;
            }
            continue;
        }

        NiThrowError("ThermalFPGA::readMPUFIFO: reading response",
                     NiFpga_ReadFifoU8(_session, bus_id, data + len, response[1], 10, NULL));

        len += response[1];

        writeDebugFile<uint8_t>("MPU>", data, len);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        remaining_time -= 1000;
    }

    std::vector<uint8_t> ret(data, data + len);

    return ret;
}

LSST::cRIO::MPUTelemetry ThermalFPGA::readMPUTelemetry(LSST::cRIO::MPU &mpu) {
    uint32_t instr_bus = 0;
    uint32_t bus_id = 0;
    switch (mpu.getBus()) {
        case SerialBusses::GLYCOOL_BUS:
            instr_bus = NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_GlycoolWrite;
            bus_id = NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_GlycoolRead;
            break;
        case SerialBusses::FLOWMETER_BUS:
            instr_bus = NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_FlowMeterWrite;
            bus_id = NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_FlowMeterRead;
            break;
        default:
            throw std::runtime_error(fmt::format("Invalid bus number - {}", mpu.getBus()));
    }

    uint8_t data[255];

    data[0] = 0;

    NiThrowError("ThermalFPGA::readMPUTelemetry: writing telemetry command",
                 NiFpga_WriteFifoU8(_session, instr_bus, data, 1, 0, NULL));

    NiThrowError("ThermalFPGA::readMPUTelemetry: reading telemetry command response",
                 NiFpga_ReadFifoU8(_session, bus_id, data, 1, 200, NULL));

    if (data[0] != 0) {
        throw std::runtime_error(fmt::format("Invalid reply from bus - {}, expected 0", data[0]));
    }

    std::cout << "Getting reply:" << std::endl;

    NiThrowError("ThermalFPGA::readMPUTelemetry: reading telemetry data",
                 NiFpga_ReadFifoU8(_session, bus_id, data, 16, 0, NULL));

    return MPUTelemetry(data);
}

void ThermalFPGA::writeCommandFIFO(uint16_t *data, size_t length, uint32_t timeout) {
    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_WriteFifoU16(_session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU16_CommandFIFO,
                                     data, length, timeout, NULL));

    writeDebugFile<uint16_t>("CMD<", data, length);
}

void ThermalFPGA::writeRequestFIFO(uint16_t *data, size_t length, uint32_t timeout) {
    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_WriteFifoU16(_session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU16_RequestFIFO,
                                     data, length, timeout, NULL));

    writeDebugFile<uint16_t>("REQ<", data, length);
}

void ThermalFPGA::readSGLResponseFIFO(float *data, size_t length, uint32_t timeout) {
    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_ReadFifoSgl(_session, NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoSgl_SGLResponseFIFO,
                                    data, length, timeout, NULL));
}

void ThermalFPGA::readU8ResponseFIFO(uint8_t *data, size_t length, uint32_t timeout) {
    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_ReadFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_U8ResponseFIFO,
                                   data, length, timeout, NULL));

    writeDebugFile<uint8_t>("U8>", data, length);
}

void ThermalFPGA::readU16ResponseFIFO(uint16_t *data, size_t length, uint32_t timeout) {
    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_ReadFifoU16(_session, NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU16_U16ResponseFIFO,
                                    data, length, timeout, NULL));

    writeDebugFile<uint16_t>("U16>", data, length);
}

float ThermalFPGA::chassisTemperature() {
    uint64_t temperature;
    cRIO::NiThrowError(
            __PRETTY_FUNCTION__,
            NiFpga_ReadU64(_session, NiFpga_ts_M1M3ThermalFPGA_IndicatorFxp_ChassisTemperature_Resource,
                           &temperature));
    return NiFpga_ConvertFromFxpToFloat(NiFpga_ts_M1M3ThermalFPGA_IndicatorFxp_ChassisTemperature_TypeInfo,
                                        temperature);
}

void ThermalFPGA::waitOnIrqs(uint32_t irqs, uint32_t timeout, bool &timedout, uint32_t *triggered) {
    static std::hash<std::thread::id> hasher;
    size_t k = hasher(std::this_thread::get_id());
    NiFpga_IrqContext contex;
    try {
        contex = _contexes.at(k);
    } catch (std::out_of_range &e) {
        NiFpga_ReserveIrqContext(_session, &contex);
        _contexes[k] = contex;
    }

    NiFpga_Bool ni_timedout = false;

    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_WaitOnIrqs(_session, contex, irqs, timeout, triggered, &ni_timedout));

    timedout = ni_timedout;
}

void ThermalFPGA::ackIrqs(uint32_t irqs) {
    NiThrowError(__PRETTY_FUNCTION__, NiFpga_AcknowledgeIrqs(_session, irqs));
}
