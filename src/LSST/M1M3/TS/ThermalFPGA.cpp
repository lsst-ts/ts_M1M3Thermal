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

#include "ThermalFPGA.h"
#include "NiFpga_ts_M1M3ThermalFPGA.h"

#include <cRIO/NiError.h>

#include <spdlog/spdlog.h>

using namespace LSST::cRIO;

namespace LSST {
namespace M1M3 {
namespace TS {

ThermalFPGA::ThermalFPGA(const char* bitfileDir) : IFPGA() {
    SPDLOG_DEBUG("ThermalFPGA: ThermalFPGA()");
    _bitfileDir = bitfileDir;
    _session = 0;
}

ThermalFPGA::~ThermalFPGA() {}

void ThermalFPGA::initialize() {
    SPDLOG_DEBUG("ThermalFPGA: initialize()");
    NiThrowError(__PRETTY_FUNCTION__, NiFpga_Initialize());
}

void ThermalFPGA::open() {
    SPDLOG_DEBUG("ThermalFPGA: open()");
    NiOpen(_bitfileDir, NiFpga_ts_M1M3ThermalFPGA, "RIO0", 0, &(_session));
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
    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_WriteFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_MPUCommandsFIFO,
                                    &bus, 1, -1, NULL));
    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_WriteFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_MPUCommandsFIFO,
                                    &len, 1, -1, NULL));
    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_WriteFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_MPUCommandsFIFO,
                                    buf.data(), len, -1, NULL));
}

void ThermalFPGA::readMPUFIFO(MPU& mpu) {
    uint8_t req = mpu.getBus() + 100;
    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_WriteFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_MPUCommandsFIFO,
                                    &req, 1, -1, NULL));

    uint8_t len;
    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_ReadFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_MPUResponseFIFO,
                                   &len, 1, -1, NULL));
    uint8_t data[len];
    uint16_t u16_data[len];

    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_ReadFifoU8(_session, NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_MPUResponseFIFO,
                                   data, len, -1, NULL));
    for (int i = 0; i < len; i++) {
        u16_data[i] = data[i];
    }
    mpu.processResponse(u16_data, len);
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

void ThermalFPGA::readU16ResponseFIFO(uint16_t* data, size_t length, uint32_t timeout) {
    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_ReadFifoU16(_session, NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU16_CommandFIFO, data,
                                    length, timeout, NULL));
}

void ThermalFPGA::waitOnIrqs(uint32_t irqs, uint32_t timeout, uint32_t* triggered) {
    static std::hash<std::thread::id> hasher;
    size_t k = hasher(std::this_thread::get_id());
    NiFpga_IrqContext contex;
    try {
        contex = _contexes.at(k);
    } catch (std::out_of_range& e) {
        NiFpga_ReserveIrqContext(_session, &contex);
        _contexes[k] = contex;
    }

    NiFpga_Bool timeouted = false;
    NiThrowError(__PRETTY_FUNCTION__,
                 NiFpga_WaitOnIrqs(_session, contex, irqs, timeout, triggered, &timeouted));
}

void ThermalFPGA::ackIrqs(uint32_t irqs) {
    NiThrowError(__PRETTY_FUNCTION__, NiFpga_AcknowledgeIrqs(_session, irqs));
}

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST
