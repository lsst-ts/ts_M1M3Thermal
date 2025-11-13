/*
 * Abstract FPGA interface.
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

#ifndef __TS_IFPGA__
#define __TS_IFPGA__

#include <chrono>
#include <memory>

#include <NiFpga.h>

#include <cRIO/FPGA.h>
#include <cRIO/MPU.h>
#include <cRIO/MPUTelemetry.h>

namespace LSST {
namespace M1M3 {
namespace TS {

namespace FPGAAddress {
constexpr uint16_t MIXING_VALVE_POSITION = 9;
constexpr uint16_t MIXING_VALVE_COMMAND = 17;
constexpr uint16_t MODBUS_A_RX = 21;
constexpr uint16_t MODBUS_A_TX = 25;
constexpr uint16_t SLOT4_DIS = 29;
constexpr uint16_t FCU_ON = 61;
constexpr uint16_t HEARTBEAT = 62;
constexpr uint16_t COOLANT_PUMP_ON = 63;
constexpr uint16_t ILC_POWER = 64;
constexpr uint16_t GLYCOLTEMP_LAST_LINE = 77;
constexpr uint16_t GLYCOLTEMP_DEBUG = 78;
constexpr uint16_t GLYCOLTEMP_TEMPERATURES = 79;
}  // namespace FPGAAddress

namespace SerialBusses {
constexpr int GLYCOOL_BUS = 1;
constexpr int FLOWMETER_BUS = 2;
constexpr int TEMPERATURE_BUS = 3;
}  // namespace SerialBusses

/**
 * Abstract FPGA Interface. Provides common parent for real and simulated FPGA.
 */
class IFPGA : public cRIO::FPGA {
public:
    IFPGA();
    virtual ~IFPGA() {}

    static IFPGA &get();

    virtual void readSGLResponseFIFO(float *data, size_t length, uint32_t timeout) = 0;
    virtual void readU8ResponseFIFO(uint8_t *data, size_t length, uint32_t timeout) = 0;

    uint16_t getTxCommand(uint8_t bus) override { return FPGAAddress::MODBUS_A_TX; }
    uint16_t getRxCommand(uint8_t bus) override { return FPGAAddress::MODBUS_A_RX; }
    uint32_t getIrq(uint8_t bus) override { return NiFpga_Irq_1; }

    float getMixingValvePosition();
    void setMixingValvePosition(float position);

    virtual float chassisTemperature() = 0;

    uint32_t getSlot4DIs();

    void setFCUPower(bool on);
    void setCoolantPumpPower(bool on);

    void setHeartbeat(bool heartbeat);

    /**
     * Fault the thermal system. Shut down pump, closes mixing valve, sets fans
     * and heaters to 0 and power down the slot.
     */
    void panic();

private:
    std::chrono::steady_clock::time_point _next_egw_powerup;
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // !__TS_IFPGA__
