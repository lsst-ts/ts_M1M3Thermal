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

void FlowMeter::read_identification() {
    clear();

    int start = TAG;
    for (; start < SERIAL_NUMBER; start += 64) {
        readHoldingRegisters(start, 21);
    }

    for (; start <= DATE_CODE; start += 64) {
        readHoldingRegisters(start, 10);
    }
}

void FlowMeter::read_telemetry() {
    clear();
    readHoldingRegisters(FLOW_RATE, 8, 255);
    readHoldingRegisters(NET_TOTALIZER, 12, 255);
    readHoldingRegisters(SIGNAL_STRENGTH, 1, 255);
}

float FlowMeter::_get_float_value(uint16_t reg) {
    union {
        uint16_t data[2];
        float dfloat;
    } buffer;
    buffer.data[0] = getRegister(reg + 1);
    buffer.data[1] = getRegister(reg);
    return buffer.dfloat;
}

double FlowMeter::_get_double_value(uint16_t reg) {
    union {
        uint16_t data[4];
        double ddouble;
    } buffer;

    for (int i = 0; i < 4; i++) {
        buffer.data[i] = getRegister(reg + 3 - i);
    }
    return buffer.ddouble;
}

std::string FlowMeter::_get_string(uint16_t reg, uint8_t len) {
    std::string ret;

    for (auto r = reg; r < reg + len; r++) {
        auto data = getRegister(r);
        if (data == 0) {
            return ret;
        }
        ret.push_back(static_cast<char>(data & 0xFF));
    }

    return ret;
}

void FlowMeterPrint::print_identification() {
    std::cout << std::setfill(' ') << std::setw(20) << "Tag: " << get_tag() << std::endl
              << std::setw(20) << "Part number: " << get_part_number() << std::endl
              << std::setw(20) << "Serial number: " << get_serial_number() << std::endl
              << std::setw(20) << "Firmware version: " << get_firmware_version() << std::endl
              << std::setw(20) << "Calibration date: " << get_calibration_date() << std::endl
              << std::setw(20) << "Data code: " << get_date_code() << std::endl
              << std::endl;
}

void FlowMeterPrint::print() {
    std::cout << std::setfill(' ') << std::fixed << std::setw(20)
              << "Signal Strength: " << get_signal_strength() << std::endl
              << std::setw(20) << "Flow Rate: " << get_flow_rate() << " l/min" << std::endl
              << std::setw(20) << "Velocity: " << get_velocity() << " m/sec" << std::endl
              << std::setw(20) << "Net Totalizer: " << get_net_totalizer() << " l" << std::endl
              << std::setw(20) << "Positive Totalizer: " << get_positive_totalizer() << " l" << std::endl
              << std::setw(20) << "Negative Totalizer: " << get_negative_totalizer() << " l" << std::endl
              << std::endl;
}
