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

#include "Commands/SAL.h"
#include "Events/AppliedSetpoints.h"
#include "Events/EngineeringMode.h"
#include "Events/ErrorCode.h"
#include "Events/FcuTargets.h"
#include "Events/SummaryState.h"
#include "Events/ThermalInfo.h"
#include "MPU/FlowMeter.h"
#include "Settings/Controller.h"
#include "Settings/GlycolPump.h"
#include "Settings/FlowMeter.h"
#include "Settings/Setpoint.h"
#include "Tasks/Controller.h"
#include "Telemetry/FinerControl.h"
#include "Telemetry/GlycolLoopTemperature.h"
#include "TSApplication.h"
#include "TSPublisher.h"

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

    Events::ErrorCode::instance().clear("CSC started");

    Telemetry::FinerControl::instance().set_target(0);
    IFPGA::get().setMixingValvePosition(0);

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

    if (Settings::FlowMeter::instance().enabled) {
        TSPublisher::instance().startFlowMeterThread();
    } else {
        SPDLOG_WARN("Skipping flow meter telemetry - the flow meter wasn't enabled in the M1M3TS config.");
    }

    if (Settings::GlycolPump::instance().enabled) {
        IFPGA::get().setCoolantPumpPower(true);
        SPDLOG_INFO("Glycol pump turned on.");
        TSPublisher::instance().startPumpThread();
    } else {
        SPDLOG_WARN("Not starting glycol pump - the glycol pump wasn't enabled in M1M3TS config.");
    }

    Events::SummaryState::set_state(MTM1M3TS_shared_SummaryStates_DisabledState);
    Events::EngineeringMode::instance().set_enabled(false);
    Events::EngineeringMode::instance().send();

    Settings::Setpoint::instance().apply_saved_setpoints();

    Events::AppliedSetpoints::instance().send();

    ackComplete();
    SPDLOG_INFO("Started");
}

void SAL_enable::execute() {
    changeAllILCsMode(ILC::Mode::Enabled);
    IFPGA::get().setFCUPower(true);

    if (Settings::GlycolPump::instance().enabled) {
        TSPublisher::instance().startupPump();
    }

    auto &applied_setpoints = Events::AppliedSetpoints::instance();

    if (applied_setpoints.is_valid() == false) {
        auto target_temp = Telemetry::GlycolLoopTemperature::instance().get_above_mirror_temperature();
        auto glycol = target_temp - 2;
        auto heaters = target_temp - 1;
        SPDLOG_INFO("Setting setpoints to {:.2f}°C {:.2f}°C - the above mirror temperature is {:.2f}°C.",
                    glycol, heaters, target_temp);
        applied_setpoints.set_applied_setpoints(glycol, heaters);
        applied_setpoints.send();
    }

    Events::SummaryState::set_state(MTM1M3TS_shared_SummaryStates_EnabledState);
    ackComplete();
    SPDLOG_INFO("Enabled");
}

void SAL_disable::execute() {
    auto zeros = std::vector<int>(cRIO::NUM_TS_ILC, 0);

    try {
        Events::FcuTargets::instance().set_FCU_heaters_fans(zeros, zeros);
    } catch (std::runtime_error &er) {
        SPDLOG_WARN(
                "Cannot set FCUs heaters and fans demands to 0/0 as the system transitions to disabled "
                "state: {}",
                er.what());
    }

    changeAllILCsMode(ILC::Mode::Disabled);

    try {
        IFPGA::get().setMixingValvePosition(0);
        IFPGA::get().setFCUPower(false);
        Telemetry::FinerControl::instance().set_target(0);
    } catch (std::runtime_error &er) {
        SPDLOG_WARN(
                "Cannot close mixing valve or power down FCUs and finer control as the system transitions to "
                "disabled state: {}",
                er.what());
    }

    Events::SummaryState::set_state(MTM1M3TS_shared_SummaryStates_DisabledState);
    ackComplete();
}

void SAL_standby::execute() {
    TSPublisher::instance().stopFlowMeterThread();
    TSPublisher::instance().stopPumpThread();
    IFPGA::get().setCoolantPumpPower(false);

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
    if (Events::SummaryState::instance().active() == false) {
        ackFailed("CSC must be in an active (disabled, enabled) state to enter or exit engineering mode.");
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
    if (Events::SummaryState::instance().enabled() == false) {
        ackFailed("CSC must be in enabled state to set FCU's heaters power.");
        return false;
    };
    return true;
}

void SAL_fanCoilsHeatersPower::execute() {
    IFPGA::get().setFCUPower(params.power);
    SPDLOG_INFO("Turned Fan Coils Heaters Power {}", params.power ? "on" : "off");
}

bool SAL_heaterFanDemand::validate() {
    if (Events::SummaryState::instance().enabled() == false) {
        ackFailed("CSC must be in enabled state to set FCU's heaters and fans demands.");
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
    if (Events::SummaryState::instance().active() == false) {
        ackFailed(
                "Manual control of the mixing valve is allowed only in active (enabled or disabled) state.");
        return false;
    }

    if (params.mixingValveTarget < 0 || params.mixingValveTarget > 100) {
        ackFailed("Mixing valve target must be between 0 and 100.");
        return false;
    }
    return true;
}

void SAL_setMixingValve::execute() {
    Telemetry::FinerControl::instance().set_target(params.mixingValveTarget);
    ackComplete();
    SPDLOG_INFO("Changed mixing valve to {:0.01f}%", params.mixingValveTarget);
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

bool SAL_coolantPumpStart::validate() {
    if (TSPublisher::instance().pump_thread == NULL) {
        ackFailed("Cannot command pump when it is powered off.");
        return false;
    }
    return true;
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
    if (Events::SummaryState::instance().active() == false) {
        ackFailed("Setpoints can be changed only in active (enabled or disabled) state.");
        return false;
    }

    auto &s_setpoint = Settings::Setpoint::instance();

    if (params.glycolSetpoint < s_setpoint.low || params.glycolSetpoint > s_setpoint.high) {
        ackFailed(fmt::format(
                "Commanded Glycol/EGW loop temperature setpoint must be between {:.02f} \u00b0C and "
                "{:.02f} \u00b0C, attempted to set to {:.02f} \u00b0C.",
                s_setpoint.low, s_setpoint.high, params.glycolSetpoint));
        return false;
    }
    if (params.heatersSetpoint < s_setpoint.low || params.heatersSetpoint > s_setpoint.high) {
        ackFailed(
                fmt::format("Commanded FCU heaters temperature setpoint must be between {:.02f} \u00b0C and "
                            "{:.02f} \ubb00C, attempted to set to {:.02f} \u00b0C.",
                            s_setpoint.low, s_setpoint.high, params.heatersSetpoint));
        return false;
    }

    float above_mirror = Telemetry::GlycolLoopTemperature::instance().get_above_mirror_temperature();
    float diff = fabs(above_mirror - params.glycolSetpoint);
    if (diff > s_setpoint.safetyRange) {
        ackFailed(fmt::format(
                "Commanded Glycol/EGW loop temperature setpoint {:.02f} \u00b0C is too far from above "
                "mirror temperature of {:.02f} \u00b0C.",
                params.glycolSetpoint, above_mirror));
        return false;
    }
    diff = fabs(above_mirror - params.heatersSetpoint);
    if (diff > s_setpoint.safetyRange) {
        ackFailed(
                fmt::format("command FCU heaters temperature setpoint {:.02f} \u00b0C is too far from above "
                            "mirror temperature of {:.02f} \u00b0C.",
                            params.heatersSetpoint, above_mirror));
        return false;
    }

    return true;
}

void SAL_applySetpoints::execute() {
    Tasks::Controller::instance().set_setpoints(params.glycolSetpoint, params.heatersSetpoint);
}
