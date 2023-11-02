/*
 * Thermal FPGA class.
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

#include <iostream>
#include <thread>

#include <spdlog/spdlog.h>

#include <cRIO/NiError.h>

#include "ThermalFPGA.h"
#include "NiFpga_ts_M1M3ThermalFPGA.h"

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

void ThermalFPGA::writeMPUFIFO(MPU& mpu) {
    auto buf = mpu.getCommandVector();
    uint8_t bus = mpu.getBus();
    uint8_t len = buf.size();
    NiThrowError(
            __PRETTY_FUNCTION__,
            NiFpga_WriteFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_SerialMultiplexRequest,
                               &bus, 1, -1, NULL));
    NiThrowError(
            __PRETTY_FUNCTION__,
            NiFpga_WriteFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_SerialMultiplexRequest,
                               &len, 1, -1, NULL));
    NiThrowError(
            __PRETTY_FUNCTION__,
            NiFpga_WriteFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_SerialMultiplexRequest,
                               buf.data(), len, -1, NULL));
}

std::vector<uint8_t> ThermalFPGA::readMPUFIFO(MPU& mpu) {
    uint8_t req = mpu.getBus() + 10;
    NiThrowError(
            __PRETTY_FUNCTION__,
            NiFpga_WriteFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_SerialMultiplexRequest,
                               &req, 1, -1, NULL));

    uint16_t len;

    NiThrowError(
            __PRETTY_FUNCTION__,
            NiFpga_ReadFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_SerialMultiplexResponse,
                              reinterpret_cast<uint8_t*>(&len), 2, 1000, NULL));
    len = ntohs(len);
    uint8_t* data = new uint8_t[len];

    NiThrowError(
            __PRETTY_FUNCTION__,
            NiFpga_ReadFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_SerialMultiplexResponse,
                              data, len, -1, NULL));
    processMPUResponse(mpu, data, len);

    std::vector<uint8_t> ret(data, data + len);

    delete[] data;

    return ret;
}

LSST::cRIO::MPUTelemetry ThermalFPGA::readMPUTelemetry(MPU& mpu) {
    uint8_t req[4] = {mpu.getBus(), 2, 254, 240};
    NiThrowError(
            __PRETTY_FUNCTION__,
            NiFpga_WriteFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_SerialMultiplexRequest,
                               req, 4, -1, NULL));

    std::vector<uint8_t> buffer;

    bool timedout;

    uint32_t irq = mpu.getIrq();

    waitOnIrqs(irq, 1000, timedout);
    if (timedout == true) {
        throw std::runtime_error("Cannot retrieve telemetry within 1 second");
    }

    uint8_t res = mpu.getBus() + 10;
    NiThrowError(
            __PRETTY_FUNCTION__,
            NiFpga_WriteFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_SerialMultiplexRequest,
                               &res, 1, -1, NULL));

    uint16_t len;
    NiThrowError(
            __PRETTY_FUNCTION__,
            NiFpga_ReadFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_SerialMultiplexResponse,
                              reinterpret_cast<uint8_t*>(&len), 2, 1, NULL));

    len = ntohs(len);
    uint8_t data[len];

    NiThrowError(
            __PRETTY_FUNCTION__,
            NiFpga_ReadFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_SerialMultiplexResponse,
                              data, len, 10, NULL));

    for (int i = 0; i < len; i++) {
        buffer.push_back(data[i]);
    }

    ackIrqs(irq);

    if (buffer.size() != 10) {
        throw std::runtime_error(
                fmt::format("Invalid telemetry length - expected 10, received {}", buffer.size()));
    }

    return MPUTelemetry(buffer.data());
}

void ThermalFPGA::writeCommandFIFO(uint16_t* data, size_t length, uint32_t timeout) {
    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_WriteFifoU16(_session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU16_CommandFIFO,
                                     data, length, timeout, NULL));
}

void ThermalFPGA::writeRequestFIFO(uint16_t* data, size_t length, uint32_t timeout) {
    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_WriteFifoU16(_session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU16_RequestFIFO,
                                     data, length, timeout, NULL));
}

void ThermalFPGA::readSGLResponseFIFO(float* data, size_t length, uint32_t timeout) {
    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_ReadFifoSgl(_session, NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoSgl_SGLResponseFIFO,
                                    data, length, timeout, NULL));
}

void ThermalFPGA::readU8ResponseFIFO(uint8_t* data, size_t length, uint32_t timeout) {
    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_ReadFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_U8ResponseFIFO,
                                   data, length, timeout, NULL));
}

void ThermalFPGA::readU16ResponseFIFO(uint16_t* data, size_t length, uint32_t timeout) {
    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_ReadFifoU16(_session, NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU16_U16ResponseFIFO,
                                    data, length, timeout, NULL));
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

void ThermalFPGA::waitOnIrqs(uint32_t irqs, uint32_t timeout, bool& timedout, uint32_t* triggered) {
    static std::hash<std::thread::id> hasher;
    size_t k = hasher(std::this_thread::get_id());
    NiFpga_IrqContext contex;
    try {
        contex = _contexes.at(k);
    } catch (std::out_of_range& e) {
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
