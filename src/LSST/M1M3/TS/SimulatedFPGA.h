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

#include <cRIO/FPGA.h>
#include <cRIO/NiError.h>
#include <cRIO/ModbusBuffer.h>

namespace LSST {
namespace M1M3 {
namespace TS {

/**
 * Simulated Thermal FPGA. Simulates answers to FPGA functions.
 */
class SimulatedFPGA : public LSST::cRIO::FPGA {
public:
    SimulatedFPGA(){};
    virtual ~SimulatedFPGA(){};
    void initialize() override{};
    void open() override{};
    void close() override{};
    void finalize() override{};
    void writeCommandFIFO(uint16_t* data, size_t length, uint32_t timeout) override;
    void writeRequestFIFO(uint16_t* data, int32_t length, int32_t timeout) override;
    void readU16ResponseFIFO(uint16_t* data, int32_t length, int32_t timeout) override;
    void waitOnIrqs(uint32_t irqs, uint32_t timeout, uint32_t* triggered = NULL);
    void ackIrqs(uint32_t irqs);

private:
    LSST::cRIO::ModbusBuffer buf;
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST
