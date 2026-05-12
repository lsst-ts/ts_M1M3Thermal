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

#include <cRIO/NiError.h>

#include "Events/DriveStatus2.h"
#include "Events/EngineeringMode.h"
#include "Events/ErrorCode.h"
#include "Events/GlycolPumpStatus.h"
#include "Events/PowerStatus.h"
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
    speedFeedback = NAN;
    outputCurrent = NAN;
    busVoltage = NAN;
    outputVoltage = NAN;

    auto& pump_settings = Settings::GlycolPump::instance();

    _recovery_left_attempts = pump_settings.communicationAutoRecoverAttempts;
    _success_count = 0;

    auto now = std::chrono::steady_clock::now();

    _startup_delay_passed = now + std::chrono::seconds(pump_settings.communicationStartupDelay);
    _fail_after = now + std::chrono::seconds(pump_settings.communicationTimeout);
    _power_on_at = now + std::chrono::seconds(pump_settings.communicationRecoverPowerOff);

    _pump_comm_state = NO_COMMUNICATION;
}

PumpThread::~PumpThread() { IFPGA::get().setCoolantPumpPower(false); }

void PumpThread::run(std::unique_lock<std::mutex>& lock) {
    runCondition.wait_for(lock, std::chrono::seconds(3));

    SPDLOG_INFO("Running Pump Thread.");
    while (keepRunning) {
        auto end = std::chrono::steady_clock::now() + 2s;

        if (_run_loop() == false) {
            break;
        }

        runCondition.wait_until(lock, end);
    }

    SPDLOG_INFO("Pump Thread stopped.");
}

void PumpThread::start_pump() {
    std::lock_guard<std::mutex> lg(_requests_lock);
    _next_requests.push_back(START);
}

void PumpThread::stop_pump() {
    std::lock_guard<std::mutex> lg(_requests_lock);
    _next_requests.push_back(STOP);
}

void PumpThread::reset_pump() {
    std::lock_guard<std::mutex> lg(_requests_lock);
    _next_requests.push_back(RESET);
}

void PumpThread::set_target_frequency(float frequency) {
    std::lock_guard<std::mutex> lg(_requests_lock);
    _target_frequency = frequency;
    _next_requests.push_back(FREQ);
}

void PumpThread::startup() {
    std::lock_guard<std::mutex> lg(_requests_lock);
    auto& pump_settings = Settings::GlycolPump::instance();
    _startup_delay_passed =
            std::chrono::steady_clock::now() + std::chrono::seconds(pump_settings.communicationStartupDelay);
    _next_requests.push_back(STARTUP);
}

void PumpThread::poweron() {
    IFPGA::get().setCoolantPumpPower(false);
    auto& pump_settings = Settings::GlycolPump::instance();
    _pump_comm_state = NO_COMMUNICATION;
    _power_on_at = std::chrono::steady_clock::now() +
                   std::chrono::seconds(pump_settings.communicationRecoverPowerOff);
    {
        std::lock_guard<std::mutex> lg(_requests_lock);
        _next_requests.push_back(POWERON);
    }
}

void PumpThread::auto_recover() {
    IFPGA::get().setCoolantPumpPower(false);
    auto& pump_settings = Settings::GlycolPump::instance();
    _pump_comm_state = NO_COMMUNICATION;
    _power_on_at = std::chrono::steady_clock::now() +
                   std::chrono::seconds(pump_settings.communicationRecoverPowerOff);
    {
        std::lock_guard<std::mutex> lg(_requests_lock);
        _next_requests.push_back(AUTO_RECOVER);
    }
}

void PumpThread::communication_check() {
    std::unique_lock<std::mutex> _lg(_requests_lock);
    if (_pump_comm_state == FAILED) {
        _lg.unlock();
        auto_recover();
    }
}

bool PumpThread::_run_loop() {
    request_type n_r = NOP;

    auto& pump_settings = Settings::GlycolPump::instance();

    try {
        vfd.clear();

        // don't read pump status when pump is powered off
        if (Events::PowerStatus::instance().pump_on() == false) {
            _transport->flush();
            return true;
        }

        n_r = _check_commands();

        if (n_r == POWERON || n_r == AUTO_RECOVER) {
            return true;
        }

        if (std::chrono::steady_clock::now() < _startup_delay_passed) {
            return true;
        }

        vfd.readInfo();

        _transport->commands(vfd, 3s, this);

        if (vfd.getCommandedFrequency() == vfd.getTargetFrequency() && vfd.get_speed_feedback() < 3000) {
            if (_recovery_left_attempts != pump_settings.communicationAutoRecoverAttempts) {
                SPDLOG_INFO("Pump running OK, resetting counter for recovery attempts.");
                _recovery_left_attempts = pump_settings.communicationAutoRecoverAttempts;
                _pump_comm_state = COMMUNICATION_OK;
            }
        }

        if (vfd.get_speed_feedback() < 3000) {
            Events::GlycolPumpStatus::instance().update(&vfd);
            Events::DriveStatus2::instance().set(vfd.get_drive_status_2());

            commandedFrequency = vfd.getCommandedFrequency();
            targetFrequency = vfd.getTargetFrequency();
            outputFrequency = vfd.getOutputFrequency();
            speedFeedback = vfd.get_speed_feedback();
            outputCurrent = vfd.getOutputCurrent();
            busVoltage = vfd.getDCBusVoltage();
            outputVoltage = vfd.getOutputVoltage();

            _fail_after = std::chrono::steady_clock::now() +
                          std::chrono::seconds(pump_settings.communicationTimeout);

            salReturn ret = TSPublisher::SAL()->putSample_glycolPump(this);
            if (ret != SAL__OK) {
                SPDLOG_WARN("Cannot send VFD: {}", ret);
            }
            _success_count++;

            // transtion from NOP to runnning
            if (vfd.getCommandedFrequency() == vfd.getTargetFrequency() && vfd.getTargetFrequency() > 0 &&
                vfd.get_speed_feedback() < 3000) {
                if (_pump_comm_state != RUNNING) {
                    SPDLOG_INFO("EGW pump running fine, frequency {}.", vfd.getCommandedFrequency());
                    _pump_comm_state = RUNNING;
                }
            }
        }
    } catch (LSST::cRIO::NiError& ni_error) {
        Events::SummaryState::instance().fail(
                Events::ErrorCode::EGWPump,
                fmt::format("Cannot communicate with the EGW pump - National Instruments Error: {}. "
                            "Mostly likely CSC has to be restarted to recover.",
                            ni_error.what()),
                "");
    } catch (std::exception& ex) {
        _success_count = 0;

        if (Events::EngineeringMode::instance().is_enabled() == true) {
            SPDLOG_WARN("VFD controller error {} occured in engineering mode, ignored.", ex.what());
            return true;
        }
        if (_fail_after < std::chrono::steady_clock::now()) {
            std::lock_guard<std::mutex> lg(_requests_lock);
            _pump_comm_state = FAILED;
            SPDLOG_WARN("Cannot communicate with the EGW pump for more than {}s, last error was {}.",
                        pump_settings.communicationTimeout, ex.what());
            return true;
        }
        if (_pump_comm_state == RUNNING) {
            SPDLOG_WARN("VFD controller communication error while pump was running: {}, ignored.", ex.what());
            return true;
        }
        if (_pump_comm_state == FAILED) {
            SPDLOG_INFO("VFD communication cannot be recovered: {}.", ex.what());
            return true;
        }
        if (_recovery_left_attempts > 0) {
            SPDLOG_INFO("Queing again failed auto-recover sequence - try {}/{}. Error {}.",
                        pump_settings.communicationAutoRecoverAttempts + 1 - _recovery_left_attempts,
                        pump_settings.communicationAutoRecoverAttempts, ex.what());
            _recovery_left_attempts--;
            auto_recover();
        }
        if (n_r != NOP) {
            Events::SummaryState::instance().fail(
                    Events::ErrorCode::EGWPumpStartup,
                    fmt::format("Run out of allowed auto-recovery attempts {} - cannot start the EGW "
                                "pump.",
                                pump_settings.communicationAutoRecoverAttempts),
                    "");
            return false;
        }
        try {
            auto buf = _transport->read(200, 2s, this);
            if (!(buf.empty())) {
                SPDLOG_ERROR("Read \"{}\" after error.", Modbus::hexDump(buf));
            }
        } catch (std::exception& ex) {
            Events::SummaryState::instance().fail(
                    Events::ErrorCode::EGWPump,
                    fmt::format("Cannot read remaining bytes from the EGW pump: {}.", ex.what()), "");
            return false;
        }
    }
    return true;
}

request_type PumpThread::_check_commands() {
    request_type n_r = NOP;

    std::lock_guard<std::mutex> lg(_requests_lock);

    auto& pump_settings = Settings::GlycolPump::instance();

    if (not(_next_requests.empty())) {
        n_r = _next_requests.front();
        bool keep = false;
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
                vfd.set_frequency(_target_frequency);
                break;
            case STARTUP:
                if (_success_count > 2) {
                    auto freq = pump_settings.startupFrequency;
                    vfd.reset();
                    vfd.set_frequency(freq);
                    vfd.start();

                    SPDLOG_INFO("Commanded pump to start at frequency {} Hz.", freq);
                } else {
                    // repeat poweron - but don't lock again mutex
                    keep = true;
                }
                break;
            case POWERON:
                // power on when requested in engineering mode
                if (std::chrono::steady_clock::now() > _power_on_at ||
                    Events::EngineeringMode::instance().is_enabled() == true) {
                    IFPGA::get().setCoolantPumpPower(true);
                    _startup_delay_passed = std::chrono::steady_clock::now() +
                                            std::chrono::seconds(pump_settings.communicationStartupDelay);
                } else {
                    keep = true;
                }
                break;
            case AUTO_RECOVER:
                if (std::chrono::steady_clock::now() > _power_on_at) {
                    IFPGA::get().setCoolantPumpPower(true);
                    _startup_delay_passed = std::chrono::steady_clock::now() +
                                            std::chrono::seconds(pump_settings.communicationStartupDelay);
                    if (Events::SummaryState::instance().enabled() &&
                        !Events::EngineeringMode::instance().is_enabled()) {
                        _next_requests.push_back(STARTUP);
                    }
                } else {
                    keep = true;
                }
                break;
            case NOP:
                break;
        }

        if (keep == false) {
            _next_requests.pop_front();
        }

        if (n_r == NOP) {
            return n_r;
        }

        _transport->commands(vfd, 2s, this);
    }

    return n_r;
}
