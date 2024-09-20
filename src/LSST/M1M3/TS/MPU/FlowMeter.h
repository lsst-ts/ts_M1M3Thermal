/*
 * FlowMeter MPU
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

#ifndef __TS_MPU_FLOWMETER__
#define __TS_MPU_FLOWMETER__

#include <cRIO/MPU.h>

namespace LSST {
namespace M1M3 {
namespace TS {

/**
 * Reads FlowMeter values.
 * [Documentation](https://rubinobs.atlassian.net/wiki/spaces/LTS/pages/50081742/Datasheets?preview=/50081742/50505733/FDT40%20Users%20Guide.pdf)
 * [Datasheets](https://confluence.lsstcorp.org/display/LTS/Datasheets).
 */
class FlowMeter : public cRIO::MPU {
public:
    FlowMeter(uint8_t bus) : MPU(bus, 1) {}

    /**
     * Push calls to readout FlowMeter registers.
     */
    void readInfo();

    uint16_t getSignalStrength() { return getRegister(5500); }
    double getFlowRate() { return _getFloatValue(1600); }
    double getNetTotalizer() { return _getDoubleValue(2600); }
    double getPositiveTotalizer() { return _getDoubleValue(2604); }
    double getNegativeTotalizer() { return _getDoubleValue(2608); }

private:
    float _getFloatValue(uint16_t reg);
    double _getDoubleValue(uint16_t reg);
};

/**
 * Add print() method to print readout values.
 */
class FlowMeterPrint : public FlowMeter {
public:
    FlowMeterPrint(uint8_t bus) : FlowMeter(bus) {}

    void print();
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif /* __TS_MPU_FLOWMETER__ */
