/*
 * Glycol Pump VFD MPU
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

#ifndef __TS_MPU_VFD__
#define __TS_MPU_VFD__

#include <chrono>

#include <cRIO/MPU.h>

using namespace std::chrono_literals;

namespace LSST {
namespace M1M3 {
namespace TS {

/**
 * Glycol pump VFD (Variable Frequency Drive) control.
 * [Documentation](https://confluence.lsstcorp.org/display/LTS/Datasheets?preview=/154697856/154697879/VFD%20Users%20Guide.pdf).
 * [Datasheets](https://confluence.lsstcorp.org/display/LTS/Datasheets).
 */
class VFD : public cRIO::MPU {
public:
    VFD() : MPU(100) {}

    void readInfo();

    /**
     * Starts pump - make sure it is running.
     */
    void start() { presetHoldingRegister(0x2000, 0x1a); }

    /**
     * Stops pump - stop its motor.
     */
    void stop() { presetHoldingRegister(0x2000, 0x01); }

    /**
     * Reset pump state - clear all errors.
     */
    void resetCommand() { presetHoldingRegister(0x2000, 0x08); }

    /**
     * Set pump output frequency.
     */
    void setFrequency(float freq) { presetHoldingRegister(0x2001, freq * 100.0f); }

    /**
     * Returns pump status.
     */
    uint16_t getStatus() { return getRegister(0x2000); }

    float getCommandedFrequency() { return getRegister(0x2001) / 100.0f; }

    uint16_t getVelocityPositionBits() { return getRegister(0x2100); }
    uint16_t getDriveErrorCodes() { return getRegister(0x2101); }
    float getTargetFrequency() { return getRegister(0x2102) / 100.0f; }
    float getOutputFrequency() { return getRegister(0x2103) / 100.0f; }
    float getOutputCurrent() { return getRegister(0x2104) / 100.0f; }
    uint16_t getDCBusVoltage() { return getRegister(0x2105); }
    float getOutputVoltage() { return getRegister(0x2106) / 10.0f; }

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

    static const char *getDriveError(uint16_t code);
};

class VFDPrint : public VFD {
public:
    void print();
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif /* __TS_MPU_VFD__ */
