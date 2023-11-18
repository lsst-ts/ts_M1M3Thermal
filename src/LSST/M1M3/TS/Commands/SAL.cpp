/*
 * SAL command.
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

#include <IFPGA.h>

#include <cRIO/ControllerThread.h>

#include <Commands/SAL.h>
#include <Events/EngineeringMode.h>
#include <Events/SummaryState.h>
#include <Events/ThermalInfo.h>
#include <Settings/Controller.h>
#include <Settings/GlycolPump.h>
#include <Settings/MixingValve.h>
#include <TSApplication.h>
#include <TSPublisher.h>

using namespace LSST::cRIO;
using namespace LSST::M1M3::TS;
using namespace LSST::M1M3::TS::Commands;
using namespace MTM1M3TS;

void changeAllILCsMode(uint16_t mode) {
    TSApplication::ilc()->clear();
    TSApplication::instance().callFunctionOnIlcs(
            [mode](uint8_t address) -> void { TSApplication::ilc()->changeILCMode(address, mode); });

    IFPGA::get().ilcCommands(*TSApplication::ilc());
}

bool SAL_start::validate() {
    if (params.configurationOverride.empty()) {
        params.configurationOverride = "Default";
    }
    if (params.configurationOverride[0] == '_') {
        SPDLOG_ERROR("configurationOverride argument shall not start with _");
        return false;
    }
    return true;
}

void SAL_start::execute() {
    SPDLOG_INFO("Starting, settings={}", params.configurationOverride);
    Settings::Controller::instance().load(params.configurationOverride);

    if (Settings::GlycolPump::instance().enabled) {
        IFPGA::get().setCoolantPumpPower(true);
        SPDLOG_INFO("Glycol pump turned on.");
    }

    changeAllILCsMode(ILC::ILCMode::Disabled);

    TSApplication::ilc()->clear();
    TSApplication::instance().callFunctionOnIlcs(
            [](uint8_t address) -> void { TSApplication::ilc()->reportServerID(address); });

    IFPGA::get().ilcCommands(*TSApplication::ilc());

    Events::ThermalInfo::instance().log();

    Events::SummaryState::setState(MTM1M3TS_shared_SummaryStates_DisabledState);
    ackComplete();
    SPDLOG_INFO("Started");
}

void SAL_enable::execute() {
    changeAllILCsMode(ILC::ILCMode::Enabled);
    IFPGA::get().setFCUPower(true);

    Events::SummaryState::setState(MTM1M3TS_shared_SummaryStates_EnabledState);
    ackComplete();
    SPDLOG_INFO("Enabled");
}

void SAL_disable::execute() {
    changeAllILCsMode(ILC::ILCMode::Disabled);
    IFPGA::get().setFCUPower(false);
    IFPGA::get().setCoolantPumpPower(false);

    Events::SummaryState::setState(MTM1M3TS_shared_SummaryStates_DisabledState);
    ackComplete();
}

void SAL_standby::execute() {
    changeAllILCsMode(ILC::ILCMode::ClearFaults);
    changeAllILCsMode(ILC::ILCMode::Standby);
    Events::SummaryState::setState(MTM1M3TS_shared_SummaryStates_StandbyState);
    ackComplete();
    SPDLOG_INFO("Standby");
}

void SAL_exitControl::execute() {
    LSST::cRIO::ControllerThread::setExitRequested();
    ackComplete();
}

bool SAL_setEngineeringMode::validate() {
    if (Events::SummaryState::instance().enabled() == false) {
        return false;
    }
    return true;
}

void SAL_setEngineeringMode::execute() {
    Events::EngineeringMode::instance().setEnabled(params.enableEngineeringMode);
    ackComplete();
    SPDLOG_INFO("{} Engineering Mode", params.enableEngineeringMode ? "Entered" : "Exited");
}

bool SAL_heaterFanDemand::validate() { return Events::EngineeringMode::instance().isEnabled(); }

void SAL_heaterFanDemand::execute() {
    TSApplication::ilc()->clear();
    TSApplication::ilc()->broadcastThermalDemand(params.heaterPWM, params.fanRPM);
    IFPGA::get().ilcCommands(*TSApplication::ilc());
    ackComplete();
    SPDLOG_INFO("Changed heaters and fans demand");
}

bool SAL_setMixingValve::validate() {
    if (params.mixingValveTarget < 0 || params.mixingValveTarget > 100) {
        return false;
    }
    return true;
}

void SAL_setMixingValve::execute() {
    float target = Settings::MixingValve::instance().percentsToCommanded(params.mixingValveTarget);
    IFPGA::get().setMixingValvePosition(target);
    ackComplete();
    SPDLOG_INFO("Changed mixing valve to {:0.01f}% ({:0.02})", params.mixingValveTarget, target);
}

bool SAL_coolantPumpPower::validate() { return Events::EngineeringMode::instance().isEnabled(); }

void SAL_coolantPumpPower::execute() {
    IFPGA::get().setCoolantPumpPower(params.power);
    ackComplete();
    SPDLOG_INFO("Glycol coolant pump powered {}", params.power ? "on" : "off");
}

void SAL_coolantPumpStart::execute() {
    IFPGA::get().next_vfd->start();
    ackComplete();
    SPDLOG_INFO("Glycol coolant pump started");
}

void SAL_coolantPumpStop::execute() {
    IFPGA::get().next_vfd->stop();
    ackComplete();
    SPDLOG_INFO("Glycol coolant pump stopped");
}

bool SAL_coolantPumpFrequency::validate() {
    if (params.targetFrequency < 0) {
        SPDLOG_WARN("Target frequency must be bigger than 0 Hz, was {:+0.02f} Hz", params.targetFrequency);
        return false;
    }
    return true;
}

void SAL_coolantPumpFrequency::execute() {
    IFPGA::get().next_vfd->setFrequency(params.targetFrequency);
    ackComplete();
    SPDLOG_INFO("Changed coolant pump target frequency to {:0.02f} Hz", params.targetFrequency);
}

void SAL_coolantPumpReset::execute() {
    IFPGA::get().next_vfd->resetCommand();
    ackComplete();
    SPDLOG_INFO("Coolant pump reseted");
}
