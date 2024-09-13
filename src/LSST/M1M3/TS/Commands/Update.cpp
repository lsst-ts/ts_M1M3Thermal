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

#include "Events/EnabledILC.h"
#include "Events/GlycolPumpStatus.h"
#include "Events/Heartbeat.h"
#include "Events/SummaryState.h"

#include "Settings/FlowMeter.h"
#include "Settings/GlycolPump.h"

#include "Telemetry/GlycolLoopTemperature.h"
#include "Telemetry/MixingValve.h"
#include "Telemetry/ThermalData.h"
#include "Telemetry/VFDSAL.h"

#include "TSApplication.h"

using namespace LSST::M1M3::TS::Commands;
using namespace std::chrono_literals;

constexpr auto default_period = 500ms;

LSST::cRIO::task_return_t Update::run() {
    SPDLOG_TRACE("Commands::Update execute");

    _sendFCU();

    _sendGlycolLoopTemperature();
    _sendMixingValve();

    Events::EnabledILC::instance().send();

    if (Settings::FlowMeter::instance().enabled) {
        _sendFlowMeter();
    }

    if (Settings::GlycolPump::instance().enabled) {
        _sendVFD();
    }

    SPDLOG_TRACE("Commands::Update leaving execute");

    return Task::DONT_RESCHEDULE;
}

void Update::_sendGlycolLoopTemperature() {
    try {
        Telemetry::GlycolLoopTemperature::instance().update();
        Telemetry::GlycolLoopTemperature::instance().send();
    } catch (std::exception &e) {
        SPDLOG_WARN("Cannot poll Glycol loop: {}", e.what());
    }
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
        TSApplication::ilc()->clear();

        TSApplication::instance().callFunctionOnIlcs([](uint8_t address) -> void {
            if (Events::SummaryState::instance().enabled()) {
                TSApplication::ilc()->reportThermalStatus(address);
            } else {
                TSApplication::ilc()->reportServerStatus(address);
            }
        });

        IFPGA::get().ilcCommands(*TSApplication::ilc(), 400);

        Telemetry::ThermalData::instance().send();
    } catch (std::exception &e) {
        SPDLOG_WARN("Cannot poll FCU: {}", e.what());
    }
}

void Update::_sendFlowMeter() {
#if 0
    try {
        bool finished = IFPGA::get().flowMeter->runLoop(IFPGA::get());
        if (finished) {
            IFPGA::get().setNextFlowMeter();
        }
    } catch (std::exception &e) {
        SPDLOG_WARN("Cannot poll Flow Meter: {}", e.what());
        IFPGA::get().setNextFlowMeter();
    }
#endif
}

void Update::_sendVFD() {
#if 0
    try {
        bool finished = IFPGA::get().vfd->runLoop(IFPGA::get());
        if (finished) {
            IFPGA::get().setNextVFD();
        }
    } catch (std::exception &e) {
        SPDLOG_WARN("Cannot poll VFD: {}", e.what());
        IFPGA::get().setNextVFD();
    }
#endif
}
