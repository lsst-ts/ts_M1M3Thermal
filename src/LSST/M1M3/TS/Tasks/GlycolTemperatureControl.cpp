/*
 * Tasks controlling mixing valve position.
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

#include "Events/AppliedSetpoints.h"
#include "IFPGA.h"
#include "Settings/MixingValve.h"
#include "Settings/Setpoint.h"
#include "Tasks/GlycolTemperatureControl.h"
#include "Telemetry/GlycolLoopTemperature.h"

using namespace LSST::M1M3::TS::Tasks;

GlycolTemperatureControl::GlycolTemperatureControl() {}

LSST::cRIO::task_return_t GlycolTemperatureControl::run() {
    auto mirror_loop = Telemetry::GlycolLoopTemperature::instance().getMirrorLoopAverage(
            Settings::Setpoint::instance().glycolSupplyPercentage / 100.0);
    float target_glycol_temp = Events::AppliedSetpoints::instance().getAppliedGlycolSetpoint();

    float diff = mirror_loop - target_glycol_temp;
    auto tolerance = Settings::Setpoint::instance().tolerance;

    auto mixing_valve_step = Settings::Setpoint::instance().mixingValveStep;

    if (diff > tolerance) {
        target_mixing_valve += mixing_valve_step;
    } else if (diff < -tolerance) {
        target_mixing_valve -= mixing_valve_step;
    } else {
        return Settings::Setpoint::instance().timestep * 1000.0;
    }

    if (target_mixing_valve > 100.0) {
        target_mixing_valve = 100.0;
    } else if (target_mixing_valve < 0) {
        target_mixing_valve = 0;
    }

    float target_v = Settings::MixingValve::instance().percentsToCommanded(target_mixing_valve);

    SPDLOG_INFO(
            "TemperatureControlLoop: new valve position is {:.1f}% ({:0.04f}), temperature difference was "
            "{:+.3f}",
            target_mixing_valve, target_v, diff);

    IFPGA::get().setMixingValvePosition(target_v);

    return Settings::Setpoint::instance().timestep * 1000.0;
}
