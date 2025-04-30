/*
 * SAL command.
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

#include <IFPGA.h>

#include <cRIO/ControllerThread.h>

#include <Commands/SAL.h>
#include <Events/AppliedSetpoints.h>
#include <Events/EngineeringMode.h>
#include <Events/FcuTargets.h>
#include <Events/SummaryState.h>
#include <Events/ThermalInfo.h>
#include <Settings/Controller.h>
#include <Settings/GlycolPump.h>
#include <Settings/MixingValve.h>
#include <Settings/Setpoint.h>
#include "TSApplication.h"
#include <TSPublisher.h>

using namespace LSST::cRIO;
using namespace LSST::M1M3::TS;
using namespace LSST::M1M3::TS::Commands;
using namespace MTM1M3TS;

void changeAllILCsMode(uint16_t mode) {
    TSApplication::ilc()->clear();
    TSApplication::instance().callFunctionOnAllIlcs(
            [mode](uint8_t address) -> void { TSApplication::ilc()->changeILCMode(address, mode); });

    try {
        IFPGA::get().ilcCommands(*TSApplication::ilc(), 1000);
    } catch (std::exception &ex) {
        SPDLOG_WARN(ex.what());
    }
}

bool SAL_start::validate() {
    if (params.configurationOverride.empty()) {
        params.configurationOverride = "Default";
    }
    if (params.configurationOverride[0] == '_') {
        ackFailed("configurationOverride argument shall not start with _");
        return false;
    }
    return true;
}

void SAL_start::execute() {
    SPDLOG_INFO("Starting, settings={}", params.configurationOverride);
    Settings::Controller::instance().load(params.configurationOverride);

    try {
        changeAllILCsMode(ILC::Mode::Disabled);

        TSApplication::ilc()->clear();
        TSApplication::instance().callFunctionOnAllIlcs(
                [](uint8_t address) -> void { TSApplication::ilc()->reportServerID(address); });

        IFPGA::get().ilcCommands(*TSApplication::ilc(), 1000);

        Events::ThermalInfo::instance().log();
        Events::FcuTargets::instance().send();
    } catch (std::exception &ex) {
        ackFailed(fmt::format("Cannot communicate with FCU's ILCs on startup: {}", ex.what()));
    }

    TSPublisher::instance().startFlowMeterThread();

    if (Settings::GlycolPump::instance().enabled) {
        IFPGA::get().setCoolantPumpPower(true);
        SPDLOG_INFO("Glycol pump turned on.");
        TSPublisher::instance().startPumpThread();
    }

    Events::SummaryState::set_state(MTM1M3TS_shared_SummaryStates_DisabledState);
    Events::EngineeringMode::instance().send();
    ackComplete();
    SPDLOG_INFO("Started");
}

void SAL_enable::execute() {
    changeAllILCsMode(ILC::Mode::Enabled);
    IFPGA::get().setFCUPower(true);

    Events::SummaryState::set_state(MTM1M3TS_shared_SummaryStates_EnabledState);
    ackComplete();
    SPDLOG_INFO("Enabled");
}

void SAL_disable::execute() {
    auto zeros = std::vector<int>(cRIO::NUM_TS_ILC, 0);
    Events::FcuTargets::instance().set_FCU_heaters_fans(zeros, zeros);
    changeAllILCsMode(ILC::Mode::Disabled);
    IFPGA::get().setFCUPower(false);
    IFPGA::get().setCoolantPumpPower(false);

    Events::SummaryState::set_state(MTM1M3TS_shared_SummaryStates_DisabledState);
    ackComplete();
}

void SAL_standby::execute() {
    TSPublisher::instance().stopFlowMeterThread();
    TSPublisher::instance().stopPumpThread();

    changeAllILCsMode(ILC::Mode::ClearFaults);
    changeAllILCsMode(ILC::Mode::Standby);
    Events::SummaryState::set_state(MTM1M3TS_shared_SummaryStates_StandbyState);
    ackComplete();
    SPDLOG_INFO("Standby");
}

void SAL_exitControl::execute() {
    LSST::cRIO::ControllerThread::setExitRequested();
    ackComplete();
}

bool SAL_setEngineeringMode::validate() {
    if (Events::SummaryState::instance().enabled() == false) {
        ackFailed("To enter the engineering mode, CSC must be in enabled state.");
        return false;
    }
    return true;
}

void SAL_setEngineeringMode::execute() {
    Events::EngineeringMode::instance().set_enabled(params.enableEngineeringMode);
    ackComplete();
    SPDLOG_INFO("{} Engineering Mode", params.enableEngineeringMode ? "Entered" : "Exited");
}

bool SAL_fanCoilsHeatersPower::validate() {
    if (Events::EngineeringMode::instance().is_enabled() == false) {
        ackFailed("CSC must be in enabled state to set heater on.");
        return false;
    };
    return true;
}

void SAL_fanCoilsHeatersPower::execute() {
    IFPGA::get().setFCUPower(params.power);
    SPDLOG_INFO("Turned Fan Coils Heaters Power {}", params.power ? "on" : "off");
}

bool SAL_heaterFanDemand::validate() {
    if (Events::EngineeringMode::instance().is_enabled() == false) {
        ackFailed("CSC must be in enabled state to set heater and fan demands.");
        return false;
    };
    return true;
}

void SAL_heaterFanDemand::execute() {
    try {
        Events::FcuTargets::instance().set_FCU_heaters_fans(params.heaterPWM, params.fanRPM);
        ackComplete();
    } catch (std::exception &e) {
        ackFailed(e.what());
    }
}

bool SAL_setMixingValve::validate() {
    if (params.mixingValveTarget < 0 || params.mixingValveTarget > 100) {
        ackFailed("Mixing valve target must be between 0 and 100.");
        return false;
    }
    return true;
}

void SAL_setMixingValve::execute() {
    float target = Settings::MixingValve::instance().percentsToCommanded(params.mixingValveTarget);
    IFPGA::get().setMixingValvePosition(target);
    ackComplete();
    SPDLOG_INFO("Changed mixing valve to {:0.01f}% ({:0.05f})", params.mixingValveTarget, target);
}

bool SAL_coolantPumpPower::validate() { return Events::EngineeringMode::instance().is_enabled(); }

void SAL_coolantPumpPower::execute() {
    IFPGA::get().setCoolantPumpPower(params.power);
    if (params.power) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        TSPublisher::instance().startPumpThread();
    } else {
        TSPublisher::instance().stopPumpThread();
    }
    ackComplete();
    SPDLOG_INFO("Glycol coolant pump powered {}", params.power ? "on" : "off");
}

void SAL_coolantPumpStart::execute() {
    TSPublisher::instance().pump_thread->start_pump();
    ackComplete();
    SPDLOG_INFO("Glycol coolant pump started");
}

void SAL_coolantPumpStop::execute() {
    TSPublisher::instance().pump_thread->stop_pump();
    ackComplete();
    SPDLOG_INFO("Glycol coolant pump stopped");
}

bool SAL_coolantPumpFrequency::validate() {
    if (params.targetFrequency < 0) {
        ackFailed(fmt::format("Target frequency must be bigger than 0 Hz, was {:+0.02f} Hz",
                              params.targetFrequency));
        return false;
    }
    return true;
}

void SAL_coolantPumpFrequency::execute() {
    TSPublisher::instance().pump_thread->set_target_frequency(params.targetFrequency);
    ackComplete();
    SPDLOG_INFO("Changed coolant pump target frequency to {:0.02f} Hz", params.targetFrequency);
}

void SAL_coolantPumpReset::execute() {
    TSPublisher::instance().pump_thread->reset_pump();
    ackComplete();
    SPDLOG_INFO("Coolant pump reseted");
}

bool SAL_applySetpoints::validate() {
    if (params.glycolSetpoint < Settings::Setpoint::instance().low ||
        params.glycolSetpoint > Settings::Setpoint::instance().high) {
        ackFailed(fmt::format(
                "Glycol loop temperature setpoint must be between {} and {}, attempted to set to {}.",
                Settings::Setpoint::instance().low, Settings::Setpoint::instance().high,
                params.glycolSetpoint));
        return false;
    }
    if (params.heatersSetpoint < Settings::Setpoint::instance().low ||
        params.heatersSetpoint > Settings::Setpoint::instance().high) {
        ackFailed(fmt::format(
                "FCU heaters temperature setpoint must be between {} and {}, attempted to set to {}.",
                Settings::Setpoint::instance().low, Settings::Setpoint::instance().high,
                params.heatersSetpoint));
        return false;
    }
    return true;
}

void SAL_applySetpoints::execute() {
    Events::AppliedSetpoints::instance().setAppliedSetpoints(params.glycolSetpoint, params.heatersSetpoint);
    Events::AppliedSetpoints::instance().send();
    SPDLOG_INFO("Glycol setpoint: {:0.2f} FCU heaters setpoint: {:0.2f}", params.glycolSetpoint,
                params.heatersSetpoint);
}
