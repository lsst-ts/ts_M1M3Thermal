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
    auto mirror_loop = Telemetry::GlycolLoopTemperature::instance().getMirrorLoopAverage();
    float target_glycol_temp = Events::AppliedSetpoints::instance().getAppliedGlycolSetpoint();
    static float new_valve_position = 10.0;

    float diff = mirror_loop - target_glycol_temp;
    auto tolerance = Settings::Setpoint::instance().tolerance;

    auto mixing_valve_step = Settings::Setpoint::instance().mixingValveStep;

    if (diff > tolerance) {
        new_valve_position += mixing_valve_step;
    } else if (diff < -tolerance) {
        new_valve_position -= mixing_valve_step;
    } else {
        return 60000;
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

    return 60000;
}
