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

#include <iomanip>
#include <iostream>

#include <spdlog/spdlog.h>

#include <MPU/FlowMeter.h>

using namespace LSST::M1M3::TS;

void FlowMeter::readInfo() {
    clear();
    readHoldingRegisters(1600, 4, 255);
    readHoldingRegisters(2600, 12, 255);
    readHoldingRegisters(5500, 1, 255);
}

float FlowMeter::_getFloatValue(uint16_t reg) {
    union {
        uint16_t data[2];
        float dfloat;
    } buffer;
    buffer.data[0] = getRegister(reg + 1);
    buffer.data[1] = getRegister(reg);
    return buffer.dfloat;
}

double FlowMeter::_getDoubleValue(uint16_t reg) {
    union {
        uint16_t data[4];
        double ddouble;
    } buffer;

    for (int i = 0; i < 4; i++) {
        buffer.data[i] = getRegister(reg + 3 - i);
    }
    return buffer.ddouble;
}

void FlowMeterPrint::print() {
    std::cout << std::setfill(' ') << std::fixed << "Signal Strength: " << getSignalStrength() << std::endl
              << std::setw(20) << "Flow Rate: " << getFlowRate() << std::endl
              << std::setw(20) << "Net Totalizer: " << getNetTotalizer() << std::endl
              << std::setw(20) << "Positive Totalizer: " << getPositiveTotalizer() << std::endl
              << std::setw(20) << "Negative Totalizer: " << getNegativeTotalizer() << std::endl
              << std::endl;
}
