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

#include <MPU/FlowMeter.h>
#include <endian.h>

using namespace LSST::M1M3::TS;

void FlowMeter::poll() { readHoldingRegisters(199, 10, 255); }

float FlowMeter::_getFloatValue(uint16_t reg) {
    union {
        uint16_t data[2];
        uint32_t d32;
        float dfloat;
    } buffer;
    buffer.data[0] = getRegister(reg);
    buffer.data[1] = getRegister(reg + 1);
    buffer.d32 = le32toh(buffer.d32);
    return buffer.dfloat;
}

double FlowMeter::_getDoubleValue(uint16_t reg) {
    union {
        uint16_t data[4];
        uint64_t d64;
        double ddouble;
    } buffer;

    for (int i = 0; i < 4; i++) {
        buffer.data[i] = getRegister(reg + i);
    }
    buffer.d64 = le64toh(buffer.d64);
    return buffer.ddouble;
}
