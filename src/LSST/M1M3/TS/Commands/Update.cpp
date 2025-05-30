/*
 * Update command.
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

#include "Commands/Update.h"

#include "Events/AppliedSetpoints.h"
#include "Events/EnabledILC.h"
#include "Events/EngineeringMode.h"
#include "Events/FcuTargets.h"
#include "Events/Heartbeat.h"
#include "Events/SummaryState.h"
#include "Events/ThermalInfo.h"

#include "Settings/Heaters.h"

#include "Telemetry/MixingValve.h"
#include "Telemetry/ThermalData.h"

#include "TSApplication.h"

using namespace LSST::M1M3::TS::Commands;
using namespace std::chrono_literals;

constexpr auto default_period = 500ms;

LSST::cRIO::task_return_t Update::run() {
    SPDLOG_TRACE("Commands::Update execute");

    _sendFCU();

    _sendMixingValve();

    Events::EnabledILC::instance().send();

    SPDLOG_TRACE("Commands::Update leaving execute");

    return Task::DONT_RESCHEDULE;
}

void Update::_sendMixingValve() {
    static auto next_update = std::chrono::steady_clock::now() - 20ms;

    auto now = std::chrono::steady_clock::now();
    if (now < next_update) {
        return;
    }
    if (now - next_update > default_period / 2.0) {
        next_update = now + default_period;
    } else {
        next_update += default_period;
    }

    try {
        Telemetry::MixingValve::instance().sendPosition(IFPGA::get().getMixingValvePosition());

    } catch (std::exception &e) {
        SPDLOG_WARN("Cannot poll mixing valve: {}", e.what());
    }
}

void Update::_sendFCU() {
    /// State of the state machine handling bus recovery from power failure
    static enum { OK, FAILED, RESET_ERROR, STANDBY, SERVER_ID, DISABLED, ENABLED } _bus_state = OK;

    static auto next_update = std::chrono::steady_clock::now() - 20ms;

    auto &app = TSApplication::instance();

    auto now = std::chrono::steady_clock::now();
    if (now < next_update) {
        return;
    }
    if (now - next_update > default_period / 2.0) {
        next_update = now + default_period;
    } else {
        next_update += default_period;
    }

    try {
        Events::ThermalInfo::instance().reset();
        Telemetry::ThermalData::instance().reset();
        app.ilc()->clear();

        switch (_bus_state) {
            case OK:
                app.callFunctionOnAllIlcs([&app](uint8_t address) {
                    if (Events::SummaryState::instance().active()) {
                        app.ilc()->reportThermalStatus(address);
                    } else {
                        app.ilc()->reportServerStatus(address);
                    }
                });
                break;
            case FAILED:
                app.callFunctionOnAllIlcs([&app](uint8_t address) {
                    app.ilc()->changeILCMode(address, ILC::Mode::ClearFaults);
                });
                break;
            case RESET_ERROR:
                app.callFunctionOnAllIlcs(
                        [&app](uint8_t address) { app.ilc()->changeILCMode(address, ILC::Mode::Standby); });
                break;
            case STANDBY:
                app.callFunctionOnAllIlcs([&app](uint8_t address) { app.ilc()->reportServerID(address); });
                break;
            case SERVER_ID:
                app.callFunctionOnAllIlcs(
                        [&app](uint8_t address) { app.ilc()->changeILCMode(address, ILC::Mode::Disabled); });
                break;
            case DISABLED:
                app.callFunctionOnAllIlcs(
                        [&app](uint8_t address) { app.ilc()->changeILCMode(address, ILC::Mode::Enabled); });
                break;
            case ENABLED:
                app.callFunctionOnAllIlcs(
                        [&app](uint8_t address) { app.ilc()->reportServerStatus(address); });
                break;
            default:
                SPDLOG_ERROR("Reached invalid ILC bus state: {}", static_cast<int>(_bus_state));
                Events::SummaryState::set_state(MTM1M3TS::MTM1M3TS_shared_SummaryStates_FaultState);
                break;
        }

        IFPGA::get().ilcCommands(*app.ilc(), 800);

        auto _old_state = _bus_state;

        switch (_bus_state) {
            case OK:
                if (Events::SummaryState::instance().active()) {
                    Telemetry::ThermalData::instance().send();
                }
                break;
            case FAILED:
                _bus_state = RESET_ERROR;
                Settings::Heaters::instance().reset_FCU_PIDs();
                break;
            case RESET_ERROR:
                _bus_state = STANDBY;
                break;
            case STANDBY:
                Events::ThermalInfo::instance().log();
                _bus_state = SERVER_ID;
                break;
            case SERVER_ID:
                _bus_state = DISABLED;
                break;
            case DISABLED:
                _bus_state = ENABLED;
                break;
            case ENABLED:
                _bus_state = OK;
                break;
        }

        if (_old_state != _bus_state) {
            SPDLOG_INFO("Recovering ILCs: transitioned from {} to {}.", static_cast<int>(_old_state),
                        static_cast<int>(_bus_state));
        }

    } catch (Modbus::MissingResponse &e) {
        if (_bus_state != FAILED) {
            SPDLOG_WARN("No response from the bus, entering ILC failed state.");
            _bus_state = FAILED;
        }
    } catch (std::exception &e) {
        SPDLOG_WARN("Cannot poll FCU: {}", e.what());
    }
}
