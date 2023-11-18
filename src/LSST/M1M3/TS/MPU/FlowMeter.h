/*
 * FlowMeter MPU
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

#ifndef __TS_MPU_FLOWMETER__
#define __TS_MPU_FLOWMETER__

#include <cRIO/MPU.h>

namespace LSST {
namespace M1M3 {
namespace TS {

/**
 * Reads FlowMeter values.
 */
class FlowMeter : public cRIO::MPU {
public:
    FlowMeter(uint8_t bus, uint8_t mpu_address) : MPU(bus, mpu_address) { setLoopTimeOut(1000ms); }

    void loopWrite() override;

    uint16_t getSignalStrength() { return getRegister(5500); }
    double getFlowRate() { return _getFloatValue(1000); }
    double getNetTotalizer() { return _getFloatValue(2500); }
    double getPositiveTotalizer() { return _getFloatValue(2502); }
    double getNegativeTotalizer() { return _getFloatValue(2504); }

private:
    float _getFloatValue(uint16_t reg);
    double _getDoubleValue(uint16_t reg);
};

class FlowMeterPrint : public FlowMeter {
public:
    FlowMeterPrint(uint8_t bus, uint8_t mpu_address) : FlowMeter(bus, mpu_address) {}

    void loopRead(bool timedout) override;
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif /* __TS_MPU_FLOWMETER__ */
