/*
 * Abstract FPGA interface.
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

#ifndef __TS_IFPGA__
#define __TS_IFPGA__

#include <cRIO/FPGA.h>
#include <NiFpga.h>

namespace LSST {
namespace M1M3 {
namespace TS {

namespace FPGAAddress {
constexpr uint16_t MIXING_VALVE_POSITION = 9;
constexpr uint16_t MIXING_VALVE_COMMAND = 17;
constexpr uint16_t MODBUS_A_RX = 21;
constexpr uint16_t MODBUS_A_TX = 25;
constexpr uint16_t HEARTBEAT = 62;
constexpr uint16_t COOLANT_PUMP_ON = 63;
}  // namespace FPGAAddress

/**
 * Abstract FPGA Interface. Provides common parent for real and simulated FPGA.
 */
class IFPGA : public cRIO::FPGA {
public:
    IFPGA() : cRIO::FPGA(cRIO::fpgaType::TS) {}
    virtual ~IFPGA() {}

    static IFPGA& get();

    virtual void readSGLResponseFIFO(float* data, size_t length, uint32_t timeout) = 0;

    uint16_t getTxCommand(uint8_t bus) override { return FPGAAddress::MODBUS_A_TX; }
    uint16_t getRxCommand(uint8_t bus) override { return FPGAAddress::MODBUS_A_RX; }
    uint32_t getIrq(uint8_t bus) override { return NiFpga_Irq_1; }

    float getMixingValvePosition();
    void setMixingValvePosition(float position);

    void setHeartbeat(bool heartbeat);
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // !__TS_IFPGA__
