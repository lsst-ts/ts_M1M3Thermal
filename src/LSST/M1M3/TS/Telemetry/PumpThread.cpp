/*
 * VFD telemetry handling class.
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

#include <spdlog/spdlog.h>

#include "Events/ErrorCode.h"
#include "Events/GlycolPumpStatus.h"
#include "Events/SummaryState.h"
#include "IFPGA.h"
#include "TSPublisher.h"
#include "Telemetry/PumpThread.h"
#include "Settings/GlycolPump.h"

using namespace LSST::M1M3::TS::Telemetry;
using namespace std::chrono_literals;

PumpThread::PumpThread(std::shared_ptr<Transports::Transport> transport) {
    _transport = transport;
    commandedFrequency = NAN;
    targetFrequency = NAN;
    outputFrequency = NAN;
    outputCurrent = NAN;
    busVoltage = NAN;
    outputVoltage = NAN;

    _error_count = 0;
    _success_count = 0;
}

request_type PumpThread::_check_commands() {
    request_type n_r = NOP;

    std::lock_guard<std::mutex> lg(_requests_lock);

    if (not(_next_requests.empty())) {
        n_r = _next_requests.front();
        switch (n_r) {
            case START:
                vfd.start();
                break;
            case STOP:
                vfd.stop();
                break;
            case RESET:
                vfd.reset();
                break;
            case FREQ:
                vfd.setFrequency(_target_frequency);
                break;
            case STARTUP:
                if (_success_count > 2) {
                    vfd.reset();
                    vfd.setFrequency(Settings::GlycolPump::instance().startupFrequency);
                    vfd.start();
                } else if (_error_count > 30) {
                    Events::SummaryState::instance().fail(Events::ErrorCode::EGWPumpStartup,
                                                          "Cannot start the EGW pump.", "");
                }
                break;
            case NOP:
                break;
        }
        _next_requests.pop();

        if (n_r == NOP) {
            return NOP;
        }

        _transport->commands(vfd, 2s, this);
    }

    return n_r;
}

void PumpThread::run(std::unique_lock<std::mutex>& lock) {
    runCondition.wait_for(lock, std::chrono::seconds(3));

    SPDLOG_INFO("Running Pump Thread.");
    while (keepRunning) {
        auto end = std::chrono::steady_clock::now() + 2s;

        request_type n_r = NOP;

        try {
            vfd.clear();

            n_r = _check_commands();

            vfd.readInfo();

            _transport->commands(vfd, 2s, this);

            Events::GlycolPumpStatus::instance().update(&vfd);

            commandedFrequency = vfd.getCommandedFrequency();
            targetFrequency = vfd.getTargetFrequency();
            outputFrequency = vfd.getOutputFrequency();
            outputCurrent = vfd.getOutputCurrent();
            busVoltage = vfd.getDCBusVoltage();
            outputVoltage = vfd.getOutputVoltage();

            salReturn ret = TSPublisher::SAL()->putSample_glycolPump(this);
            if (ret != SAL__OK) {
                SPDLOG_WARN("Cannot send VFD: {}", ret);
            }

            _error_count = 0;
            _success_count++;
        } catch (std::exception& ex) {
            SPDLOG_ERROR("Error in running Glycol Pump thread: {}", ex.what());
            _error_count++;
            _success_count = 0;
            if (n_r == STARTUP) {
                SPDLOG_INFO("Queing again failed startup sequence.");
                startup();
            }
            auto buf = _transport->read(200, 2s, this);
            if (!(buf.empty())) {
                SPDLOG_ERROR("Read \"{}\" after error.", Modbus::hexDump(buf));
            }
        }

        runCondition.wait_until(lock, end);
    }

    SPDLOG_DEBUG("Pump Thread Stopped.");
}

void PumpThread::start_pump() {
    std::lock_guard<std::mutex> lg(_requests_lock);
    _next_requests.push(START);
}

void PumpThread::stop_pump() {
    std::lock_guard<std::mutex> lg(_requests_lock);
    _next_requests.push(STOP);
}
void PumpThread::reset_pump() {
    std::lock_guard<std::mutex> lg(_requests_lock);
    _next_requests.push(RESET);
}
void PumpThread::set_target_frequency(float frequency) {
    std::lock_guard<std::mutex> lg(_requests_lock);
    _target_frequency = frequency;
    _next_requests.push(FREQ);
}
void PumpThread::startup() {
    std::lock_guard<std::mutex> lg(_requests_lock);
    _next_requests.push(STARTUP);
}
