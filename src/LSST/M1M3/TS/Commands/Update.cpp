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

#include "Commands/Update.h"
#include "TSApplication.h"

#include "Events/EnabledILC.h"
#include "Events/Heartbeat.h"
#include "Events/SummaryState.h"

#include "Telemetry/ThermalData.h"
#include "Telemetry/MixingValve.h"

#include <cRIO/ThermalILC.h>

#include <spdlog/spdlog.h>

using namespace LSST::M1M3::TS::Commands;

void Update::execute() {
    SPDLOG_TRACE("Commands::Update execute");

    // FCU telemetry
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

    Telemetry::MixingValve::instance().sendPosition(IFPGA::get().getMixingValvePosition());

    Events::EnabledILC::instance().send();

    Events::Heartbeat::instance().tryToggle();

    SPDLOG_TRACE("Commands::Update leaving execute");
}
