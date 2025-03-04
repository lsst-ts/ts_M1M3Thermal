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

#include <chrono>

#include <spdlog/spdlog.h>

#include <cRIO/ThermalILC.h>

#include "Commands/Update.h"

#include "Events/AppliedSetpoint.h"
#include "Events/EnabledILC.h"
#include "Events/EngineeringMode.h"
#include "Events/Heartbeat.h"
#include "Events/SummaryState.h"
#include "Events/ThermalInfo.h"

#include <Settings/MixingValve.h>
#include <Settings/Setpoint.h>

#include "Telemetry/GlycolLoopTemperature.h"
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

    _temperatureControlLoop();

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

        TSApplication::instance().callFunctionOnAllIlcs([](uint8_t address) -> void {
            if (Events::SummaryState::instance().active()) {
                TSApplication::ilc()->reportThermalStatus(address);
            } else {
                TSApplication::ilc()->reportServerStatus(address);
            }
        });

        IFPGA::get().ilcCommands(*TSApplication::ilc(), 800);

        if (Events::SummaryState::instance().active()) {
            Telemetry::ThermalData::instance().send();
        }
    } catch (std::exception &e) {
        SPDLOG_WARN("Cannot poll FCU: {}", e.what());
    }
}

void Update::_temperatureControlLoop() {
    auto timestep_ms = std::chrono::milliseconds(int(Settings::Setpoint::instance().timestep * 1000.0));

    static auto next_update = std::chrono::steady_clock::now() - timestep_ms;

    auto now = std::chrono::steady_clock::now();
    if (now < next_update) {
        return;
    }

    next_update += timestep_ms;

    if (Events::SummaryState::instance().enabled() == false ||
        Events::EngineeringMode::instance().isEnabled() == true) {
        return;
    }

    auto mirrorLoop = Telemetry::GlycolLoopTemperature::instance().getMirrorLoopAverage();
    float targetTemp = Events::AppliedSetpoint::instance().getAppliedSetpoint();
    static float new_valve_position = 10.0;

    float diff = mirrorLoop - targetTemp;
    auto tolerance = Settings::Setpoint::instance().tolerance;

    auto mixingValveStep = Settings::Setpoint::instance().mixingValveStep;

    if (diff > tolerance) {
        new_valve_position += mixingValveStep;
    } else if (diff < -tolerance) {
        new_valve_position -= mixingValveStep;
    } else {
        return;
    }

    if (new_valve_position > 100.0) {
        new_valve_position = 100.0;
    } else if (new_valve_position < 0) {
        new_valve_position = 0;
    }

    SPDLOG_INFO("TemperatureControlLoop: new valve position is {:.1f}%, temperature difference was {:+.3f}",
                new_valve_position, diff);

    IFPGA::get().setMixingValvePosition(
            Settings::MixingValve::instance().percentsToCommanded(new_valve_position));
}
