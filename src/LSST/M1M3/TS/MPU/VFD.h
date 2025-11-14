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
 * Drive status 2, page 114 of the VFD manual.
 */
static const std::string str_status_2[16] = {"Jogging",
                                             "Flux Breaking",
                                             "Motor Overload",
                                             "AutoRst Ctdn",
                                             "DC Braking",
                                             "At Frequency",
                                             "Auto Tuning",
                                             "EM Braking",
                                             "Current Limit",
                                             "",
                                             "Safety S1",
                                             "Safety S2",
                                             "F111 Status",
                                             "SafeTqPermit",
                                             "",
                                             ""};

/**
 * Glycol pump VFD (Variable Frequency Drive) control.
 * [Documentation](https://confluence.lsstcorp.org/display/LTS/Datasheets?preview=/154697856/154697879/VFD%20Users%20Guide.pdf).
 * [Datasheets](https://confluence.lsstcorp.org/display/LTS/Datasheets).
 */
class VFD : public cRIO::MPU {
public:
    enum REGISTERS {
        DRIVE_STATUS_2 = 393,
        COMMAND = 0x2000,
        SET_FREQUENCY = 0x2001,
        VELOCITY_BITS = 0x2100,
        DRIVE_ERROR_CODE = 0x2101,
        FREQUENCY_COMMAND = 0x2102,
        OUTPUT_FREQUENCY = 0x2103,
        OUTPUT_CURRENT = 0x2104,
        DC_BUS_VOLTAGE = 0x2105,
        OUTPUT_VOLTAGE = 0x2106
    };

    VFD() : MPU(100) {}

    void readInfo();

    /**
     * Starts pump - make sure it is running.
     */
    void start() { presetHoldingRegister(REGISTERS::COMMAND, 0x1a); }

    /**
     * Stops pump - stop its motor.
     */
    void stop() { presetHoldingRegister(REGISTERS::COMMAND, 0x01); }

    /**
     * Reset pump state - clear all errors.
     */
    void reset() { presetHoldingRegister(REGISTERS::COMMAND, 0x08); }

    /**
     * Set pump output frequency.
     */
    void set_frequency(float freq) { presetHoldingRegister(REGISTERS::SET_FREQUENCY, freq * 100.0f); }

    uint16_t get_drive_status_2() { return getRegister(REGISTERS::DRIVE_STATUS_2); }

    /**
     * Returns pump status.
     */
    uint16_t getStatus() { return getRegister(REGISTERS::COMMAND); }

    float getCommandedFrequency() { return getRegister(REGISTERS::SET_FREQUENCY) / 100.0f; }

    uint16_t getVelocityPositionBits() { return getRegister(REGISTERS::VELOCITY_BITS); }
    uint16_t getDriveErrorCodes() { return getRegister(REGISTERS::DRIVE_ERROR_CODE); }
    float getTargetFrequency() { return getRegister(REGISTERS::FREQUENCY_COMMAND) / 100.0f; }
    float getOutputFrequency() { return getRegister(REGISTERS::OUTPUT_FREQUENCY) / 100.0f; }
    float getOutputCurrent() { return getRegister(REGISTERS::OUTPUT_CURRENT) / 100.0f; }
    uint16_t getDCBusVoltage() { return getRegister(REGISTERS::DC_BUS_VOLTAGE); }
    float getOutputVoltage() { return getRegister(REGISTERS::OUTPUT_VOLTAGE) / 10.0f; }

    void readParameters() { readHoldingRegisters(1, 50); }

    /**
     * Reads VFD registers. Starts from the "Driver Error Code" and reads all registers containing current
     * data.
     */
    void update();

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
