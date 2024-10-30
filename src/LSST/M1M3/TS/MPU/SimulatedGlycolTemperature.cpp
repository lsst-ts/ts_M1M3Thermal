/*
 * Simulated flow meter device.
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

#include <chrono>

#include <spdlog/spdlog.h>

#include <cRIO/MPU.h>

#include "SimulatedGlycolTemperature.h"

using namespace LSST::cRIO;
using namespace LSST::M1M3::TS;

SimulatedGlycolTemperature::SimulatedGlycolTemperature()
        : generator(std::chrono::system_clock::now().time_since_epoch().count()), distribution(-0.1, 0.2) {
    _bytes_written = 0;
    _bytes_read = 0;

    _temperatures[0] = 0.1234;
    for (int i = 1; i < 8; i++) {
        _temperatures[i] = i + distribution(generator);
    }
}

void SimulatedGlycolTemperature::write(const unsigned char* buf, size_t len) {
    throw std::runtime_error("The code shall not write to the glycol temperature device");
}

std::vector<uint8_t> SimulatedGlycolTemperature::read(size_t len, std::chrono::microseconds timeout,
                                                      LSST::cRIO::Thread* calling_thread) {
    std::this_thread::sleep_for(timeout - std::chrono::milliseconds(100));
    std::string ret;

    for (int i = 0; i < 8; i++) {
        if (i > 0) {
            _temperatures[i] += distribution(generator);
            ret += ",";
        } else {
            _temperatures[i]++;
        }

        ret += fmt::format("C{:02d}={:09.4f}", i + 1, _temperatures[i]);
    }

    ret += "\r\n";

    return std::vector<uint8_t>(ret.begin(), ret.end());
}

void SimulatedGlycolTemperature::commands(Modbus::BusList& bus_list, std::chrono::microseconds timeout,
                                          LSST::cRIO::Thread* calling_thread) {
    throw std::runtime_error("The code shall not command the glycol temperature device");
}

void SimulatedGlycolTemperature::flush() {}

void SimulatedGlycolTemperature::telemetry(uint64_t& write_bytes, uint64_t& read_bytes) {
    write_bytes = _bytes_written;
    read_bytes = _bytes_read;
}
