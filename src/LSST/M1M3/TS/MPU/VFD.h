/*
 * Glycol Pump VFD MPU
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

#include <cRIO/MPU.h>

namespace LSST {
namespace M1M3 {
namespace TS {

/**
 * Reads FlowMeter values.
 */
class VFD : public cRIO::MPU {
public:
    VFD(uint8_t bus, uint8_t mpu_address) : MPU(bus, mpu_address) {}

    void poll();

    uint16_t getStatus() { return getRegister(0x2000); }
    uint16_t getCommandedFrequency() { return getRegister(0x2001); }

    uint16_t getVelocityPositionBits() { return getRegister(0x2100); }
    uint16_t getDriveErrorCodes() { return getRegister(0x2101); }
    uint16_t getTargetFrequency() { return getRegister(0x2102); }
    uint16_t getOutputFrequency() { return getRegister(0x2103); }
    uint16_t getOutputCurrent() { return getRegister(0x2104); }
    uint16_t getDCBusVoltage() { return getRegister(0x2105); }
    uint16_t getOutputVoltage() { return getRegister(0x2106); }
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST
