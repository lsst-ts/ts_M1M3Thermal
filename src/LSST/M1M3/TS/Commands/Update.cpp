/*
 * Update command.
 *
 * Developed for the Vera C. Rubin Observatory Telescope & Site Software Systems.
 * This product includes software developed by the Vera C.Rubin Observatory Project
 * (https://www.lsst.org). See the COPYRIGHT file at the top-level directory of
 * this distribution for details of code ownership.
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

#include <cRIO/ThermalILC.h>

#include "Commands/Update.h"
#include "TSApplication.h"

#include "Events/EnabledILC.h"
#include "Events/Heartbeat.h"
#include "Events/SummaryState.h"

#include "Telemetry/FlowMeter.h"
#include "Telemetry/VFD.h"
#include "Telemetry/GlycolLoopTemperature.h"
#include "Telemetry/MixingValve.h"
#include "Telemetry/ThermalData.h"

using namespace LSST::M1M3::TS::Commands;

void Update::execute() {
    SPDLOG_TRACE("Commands::Update execute");

    _sendFCU();

    _sendGlycolLoopTemperature();
    _sendMixingValve();

    Events::EnabledILC::instance().send();

    Events::Heartbeat::instance().tryToggle();

    _sendFlowMeter();
    _sendVFD();

    SPDLOG_TRACE("Commands::Update leaving execute");
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
    try {
        Telemetry::MixingValve::instance().sendPosition(IFPGA::get().getMixingValvePosition());

    } catch (std::exception &e) {
        SPDLOG_WARN("Cannot poll mixing valve: {}", e.what());
    }
}

void Update::_sendFCU() {
    try {
        TSApplication::ilc()->clear();

        TSApplication::instance().callFunctionOnIlcs([](uint8_t address) -> void {
            if (Events::SummaryState::instance().enabled()) {
                TSApplication::ilc()->reportThermalStatus(address);
            } else {
                TSApplication::ilc()->reportServerStatus(address);
            }
        });

        IFPGA::get().ilcCommands(*TSApplication::ilc());

        Telemetry::ThermalData::instance().send();
    } catch (std::exception &e) {
        SPDLOG_WARN("Cannot poll FCU: {}", e.what());
    }
}

void Update::_sendFlowMeter() {
    try {
        Telemetry::FlowMeter::instance().update();
        Telemetry::FlowMeter::instance().send();
    } catch (std::exception &e) {
        SPDLOG_WARN("Cannot poll Flow Meter: {}", e.what());
    }
}

void Update::_sendVFD() {
    try {
        Telemetry::VFD::instance().update();
        Telemetry::VFD::instance().send();
    } catch (std::exception &e) {
        SPDLOG_WARN("Cannot poll VFD: {}", e.what());
    }
}
