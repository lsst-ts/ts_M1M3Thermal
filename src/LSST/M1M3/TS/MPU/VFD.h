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

#ifndef __TS_MPU_VFD__
#define __TS_MPU_VFD__

#include <cRIO/MPU.h>

namespace LSST {
namespace M1M3 {
namespace TS {

/**
 * Glycol pump VFD (Variable Frequency Drive) control.
 * [Documentation](https://confluence.lsstcorp.org/display/LTS/Datasheets?preview=/154697856/154697879/VFD%20Users%20Guide.pdf).
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

    void readParameters() { readHoldingRegisters(1, 50); }

    /**
     * Reads VFD registers.
     *
     * Register   | Content
     * ---------- | -----------------
     * 0x2101     | Drive Error Code
     * 0x2102     | Frequency Command
     * 0x2103     | Output Frequency
     * 0x2104     | Output Current
     * 0x2105     | DC-BUS Voltage
     * 0x2106     | Output Voltage
     */
    void update() { readHoldingRegisters(0x2101, 6); }

    static const char* getDriveError(uint16_t code);
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif /* __TS_MPU_VFD__ */
