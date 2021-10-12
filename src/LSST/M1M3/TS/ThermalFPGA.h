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

#ifndef __TS_THERMALFPGA__
#define __TS_THERMALFPGA__

#include <IFPGA.h>

namespace LSST {
namespace M1M3 {
namespace TS {

/**
 * Thermal FPGA. Provides functions specific for Thermal FPGA, implements
 * generic functions.
 */
class ThermalFPGA : public IFPGA {
public:
    ThermalFPGA();
    virtual ~ThermalFPGA();
    void initialize() override;
    void open() override;
    void close() override;
    void finalize() override;
    void writeMPUFIFO(cRIO::MPU& mpu) override;
    void readMPUFIFO(cRIO::MPU& mpu) override;
    void writeCommandFIFO(uint16_t* data, size_t length, uint32_t timeout) override;
    void writeRequestFIFO(uint16_t* data, size_t length, uint32_t timeout) override;
    void readSGLResponseFIFO(float *data, size_t length, uint32_t timeout) override;
    void readU16ResponseFIFO(uint16_t* data, size_t length, uint32_t timeout) override;
    void waitOnIrqs(uint32_t irqs, uint32_t timeout, uint32_t* triggered = NULL) override;
    void ackIrqs(uint32_t irqs) override;

private:
    uint32_t _session;

    std::map<size_t, NiFpga_IrqContext> _contexes;
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // !__TS_THERMALFPGA__
