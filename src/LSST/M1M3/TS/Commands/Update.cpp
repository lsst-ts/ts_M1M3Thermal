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

#include <SAL_MTM1M3TS.h>

#include <cRIO/ThermalILC.h>

#include "Commands/Update.h"

#include "Events/AppliedSetpoints.h"
#include "Events/EnabledILC.h"
#include "Events/EngineeringMode.h"
#include "Events/FcuTargets.h"
#include "Events/Heartbeat.h"
#include "Events/SummaryState.h"
#include "Events/ThermalInfo.h"

#include <Settings/MixingValve.h>
#include <Settings/Setpoint.h>

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

    if (Events::SummaryState::instance().enabled() == true &&
        Events::EngineeringMode::instance().is_enabled() == false) {
        auto timestep_ms = std::chrono::milliseconds(int(Settings::Setpoint::instance().timestep * 1000.0));

        _next_update = std::chrono::steady_clock::now() - timestep_ms;

        auto now = std::chrono::steady_clock::now();
        if (now >= _next_update) {
            _next_update += timestep_ms;
            _glycol_temperature_control_loop();
            _heaters_temperature_control_loop();
        }
    }

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
        TSApplication::ilc()->clear();

        switch (_bus_state) {
            case OK:
                TSApplication::instance().callFunctionOnAllIlcs([](uint8_t address) {
                    if (Events::SummaryState::instance().active()) {
                        TSApplication::ilc()->reportThermalStatus(address);
                    } else {
                        TSApplication::ilc()->reportServerStatus(address);
                    }
                });
                break;
            case FAILED:
                TSApplication::instance().callFunctionOnAllIlcs([](uint8_t address) {
                    TSApplication::ilc()->changeILCMode(address, ILC::Mode::ClearFaults);
                });
                break;
            case RESET_ERROR:
                TSApplication::instance().callFunctionOnAllIlcs([](uint8_t address) {
                    TSApplication::ilc()->changeILCMode(address, ILC::Mode::Standby);
                });
                break;
            case STANDBY:
                TSApplication::instance().callFunctionOnAllIlcs(
                        [](uint8_t address) { TSApplication::ilc()->reportServerID(address); });
                break;
            case SERVER_ID:
                TSApplication::instance().callFunctionOnAllIlcs([](uint8_t address) {
                    TSApplication::ilc()->changeILCMode(address, ILC::Mode::Disabled);
                });
                break;
            case DISABLED:
                TSApplication::instance().callFunctionOnAllIlcs([](uint8_t address) {
                    TSApplication::ilc()->changeILCMode(address, ILC::Mode::Enabled);
                });
                break;
            case ENABLED:
                TSApplication::instance().callFunctionOnAllIlcs(
                        [](uint8_t address) { TSApplication::ilc()->reportServerStatus(address); });
                break;
            default:
                SPDLOG_ERROR("Reached invalid ILC bus state: {}", static_cast<int>(_bus_state));
                Events::SummaryState::setState(MTM1M3TS::MTM1M3TS_shared_SummaryStates_FaultState);
                break;
        }

        IFPGA::get().ilcCommands(*TSApplication::ilc(), 800);

        auto _old_state = _bus_state;

        switch (_bus_state) {
            case OK:
                if (Events::SummaryState::instance().active()) {
                    Telemetry::ThermalData::instance().send();
                }
                break;
            case FAILED:
                _bus_state = RESET_ERROR;
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

void Update::_glycol_temperature_control_loop() {
}

void Update::_heaters_temperature_control_loop() {
    auto heaterPWM = Events::FcuTargets::instance().get_heaterPWM();
    auto fanRPM = Events::FcuTargets::instance().get_fanRPM();
    auto temperature = Telemetry::ThermalData::instance().get_absoluteTemperature();
    auto target_temperature = Events::AppliedSetpoints::instance().getAppliedHeatersSetpoint();
    std::vector<int> target_heater(cRIO::NUM_TS_ILC);
    std::vector<int> target_fan(cRIO::NUM_TS_ILC);
    for (int i = 0; i < cRIO::NUM_TS_ILC; i++) {
        if (temperature[i] < target_temperature && heaterPWM[i] < 100) {
            target_heater[i] = 255 * heaterPWM[i] / 100.0 + 1;
        } else if (heaterPWM[i] > 0) {
            target_heater[i] = 255 * heaterPWM[i] / 100.0 - 1;
        }

        if (target_heater[i] > 255) {
            target_heater[i] = 255;
        }
        if (target_heater[i] < 0) {
            target_heater[i] = 0;
        }
        target_fan[i] = 2;
    }
    TSApplication::instance().set_FCU_heaters_fans(target_heater, target_fan);
}
