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

#include <MPU/VFD.h>

using namespace LSST::M1M3::TS;

void VFD::poll() {
    readHoldingRegisters(0x2000, 3, 255);
    readHoldingRegisters(0x2100, 7, 255);
}

const char *VFD::getDriveError(uint16_t code) {
    switch (code) {
        case 0:
            return "No Fault";
        case 2:
            return "Auxiliary Input";
        case 3:
            return "Power Loss";
        case 4:
            return "Undervoltage";
        default:
            return "Unknown error";
    }
}