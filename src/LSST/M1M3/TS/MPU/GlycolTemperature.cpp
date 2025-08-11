/*
 * Glycol Temperature readout
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
#include <sstream>

#include <spdlog/spdlog.h>

#include "IFPGA.h"
#include "MPU/GlycolTemperature.h"

using namespace LSST::cRIO;
using namespace LSST::M1M3::TS;
using namespace std::chrono_literals;

GlycolTemperature::GlycolTemperature(std::shared_ptr<Transports::Transport> transport)
        : MPU(0), Thread(), _transport(transport) {
    for (int i = 0; i < 8; i++) {
        _temperatures[i] = NAN;
    }
}

void GlycolTemperature::run(std::unique_lock<std::mutex>& lock) {
    SPDLOG_DEBUG("Running Glycol Temperature thread.");

    auto last_data = std::chrono::steady_clock::now();
    int proc_error_count = 0;

    while (keepRunning) {
        auto end = std::chrono::steady_clock::now() + 100ms;

        std::vector<uint8_t> new_data;

        try {
            new_data = _transport->read(10, 400ms, this);
            receiving_error_count = 0;
        } catch (std::runtime_error& er) {
            if (receiving_error_count == 0) {
                SPDLOG_WARN("Cannot read Glycol temperature data: {}", er.what());
            }
            receiving_error_count++;

            runCondition.wait_until(lock, end);

            continue;
        }

        if (new_data.size() > 0) {
            SPDLOG_TRACE("Received temperature data: {}, so far: {}", new_data.size(), _data_buffer.size());
            _data_buffer += std::string(new_data.begin(), new_data.end());
        }

        auto new_line = _data_buffer.find('\r');

        if (new_line != std::string::npos && new_line + 1 < _data_buffer.size()) {
            {
                last_data = std::chrono::steady_clock::now();

                std::lock_guard<std::mutex> guard(_temperature_mutex);

                _last_processed = _data_buffer.substr(0, new_line + 1);
                std::stringstream ss(_last_processed);

                int i;
                for (i = 0; i < 8; i++) {
                    _temperatures[i] = NAN;
                }

                i = 0;
                while (ss.good()) {
                    char c;
                    int num;
                    char eq;
                    float v;
                    ss >> c >> num >> eq >> v;

                    // SPDLOG_TRACE("c:{} num:{} eq:{} v:{}", c, num, eq, v);

                    if (ss.good() && c == 'C' && eq == '=' && num == i + 1) {
                        _temperatures[i] = v < 900 ? v : NAN;
                        ss >> c;
                    } else {
                        break;
                    }

                    if ((!ss.good() && i < 7 && c != ',') || (i == 7 && !ss.eof())) {
                        SPDLOG_WARN("Cannot parse {} at position {}", _last_processed.substr(0, new_line),
                                    static_cast<int>(ss.tellg()));
                        break;
                    }

                    i++;
                }
                _data_buffer = _data_buffer.substr(new_line + 2);
            }

            // updates must be called without hold of the _temperature_mutex
            updated();
            proc_error_count = 0;
        } else {
            if (std::chrono::steady_clock::now() > last_data + 4s) {
                if (proc_error_count == 0) {
                    SPDLOG_WARN("Empty glycol temp buffer: {}", _data_buffer);
                }
                proc_error_count++;
            }
        }

        runCondition.wait_until(lock, end);
    }
    SPDLOG_DEBUG("Glycol Temperature Thread stopped.");
}

std::vector<float> GlycolTemperature::getTemperatures() {
    std::lock_guard<std::mutex> guard(_temperature_mutex);
    return std::vector<float>(_temperatures, _temperatures + 8);
}

std::string GlycolTemperature::getDataBuffer() {
    std::lock_guard<std::mutex> guard(_temperature_mutex);
    return _last_processed;
}
