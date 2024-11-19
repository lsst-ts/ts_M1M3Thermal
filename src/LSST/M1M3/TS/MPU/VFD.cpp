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

#include <iomanip>
#include <iostream>

#include <spdlog/spdlog.h>

#include <MPU/VFD.h>

using namespace LSST::M1M3::TS;

void VFD::readInfo() {
    readHoldingRegisters(0x2000, 3, 1500);
    readHoldingRegisters(0x2100, 7, 1500);
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

void VFDPrint::print() {
    SPDLOG_TRACE("Requesting glycol pump VFD registers");

    // status bits
    static const std::string status[16] = {"Ready",
                                           "Active (Running)",
                                           "Cmd Forward",
                                           "Rotating Forward",
                                           "Accelerating",
                                           "Decelerating",
                                           "",
                                           "Faulted",
                                           "At Reference",
                                           "Main Freq Controlled by Active Comm",
                                           "Operation Cmd Controlled by Active Comm",
                                           "Parameters have been locked",
                                           "Digital input 1 Status (DigIn TermBlk 05)"
                                           "Digital input 2 Status (DigIn TermBlk 06)"
                                           "Digital input 3 Status (DigIn TermBlk 07)"
                                           "Digital input 4 Status (DigIn TermBlk 08)"};

    uint16_t bits = getVelocityPositionBits();

    std::cout << std::setfill(' ') << std::setw(20) << "Status: " << "0x" << std::hex << getStatus()
              << std::endl
              << std::setw(20) << "Commanded Freq.: " << std::fixed << std::setprecision(2)
              << getCommandedFrequency() << std::endl
              << std::setw(20) << "Vel./Pos. Bits: " << std::hex << bits << std::dec << std::endl;

    for (int i = 0; i < 16; i++) {
        if (bits & 0x01)
            std::cout << "         " << (bits == 0x01 ? "┗" : "┣") << "━━▶ " << status[i] << std::endl;
        bits >>= 1;
    }

    std::cout << std::setw(20) << "Drive Error Codes: " << getDriveErrorCodes() << std::endl
              << std::setw(20) << "Target Frequency: " << std::fixed << std::setprecision(2)
              << getTargetFrequency() << std::endl
              << std::setw(20) << "Output Frequency: " << getOutputFrequency() << std::endl
              << std::setw(20) << "Output Current: " << getOutputCurrent() << std::endl
              << std::setw(20) << "DC Bus Voltage: " << std::dec << getDCBusVoltage() << std::endl
              << std::setw(20) << "Output Voltage: " << std::fixed << std::setprecision(1)
              << getOutputVoltage() << std::endl
              << std::endl;
}
