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
#include "TSPublisher.h"

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
    NiOpen("/var/lib/M1M3TS", NiFpga_ts_M1M3ThermalFPGA, "RIO0", NiFpga_OpenAttribute_NoRun, &(_session));
    uint32_t ni_state;
    NiThrowError(__PRETTY_FUNCTION__, "NiFpga_GetFpgaViState", NiFpga_GetFpgaViState(_session, &ni_state));
    switch (ni_state) {
        case NiFpga_FpgaViState_Invalid:
            NiThrowError(__PRETTY_FUNCTION__, "NiFpga_Download", NiFpga_Download(_session));
        case NiFpga_FpgaViState_NaturallyStopped:
        case NiFpga_FpgaViState_NotRunning:
        case NiFpga_FpgaViState_Running:
            NiThrowError(__PRETTY_FUNCTION__, "NiFpga_Reset", NiFpga_Reset(_session));
            NiThrowError(__PRETTY_FUNCTION__, "NiFpga_Run", NiFpga_Run(_session, 0));
            break;
        default:
            throw std::runtime_error(fmt::format("Unknow FPGA state: {}", ni_state));
    }
    setMixingValvePosition(0);
    setCoolantPumpPower(false);
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
