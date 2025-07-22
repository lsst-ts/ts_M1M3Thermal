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

GlycolTemperatureControl::GlycolTemperatureControl()
        : target_pid(Settings::MixingValve::instance().pid_parameters, 0, 100) {}

LSST::cRIO::task_return_t GlycolTemperatureControl::run() {
    auto &glycol_temp = Telemetry::GlycolLoopTemperature::instance();
    auto &s_setpoint = Settings::Setpoint::instance();

    auto mirror_loop = glycol_temp.get_mirror_loop_average(s_setpoint.glycolSupplyPercentage / 100.0);
    float target_glycol_temp = Events::AppliedSetpoints::instance().get_applied_glycol_setpoint();

    if (isnan(target_glycol_temp) || isnan(mirror_loop)) {
        SPDLOG_INFO("Glycol targets not set, ending the loop.");
        return Task::DONT_RESCHEDULE;
    }

    float diff = mirror_loop - target_glycol_temp;

    float target_mixing_valve = round(target_pid.process(target_glycol_temp, mirror_loop) / 5.0) * 5.0;

    target_mixing_valve = std::max(0.0f, std::min(target_mixing_valve, 100.0f));

    float target_v = Settings::MixingValve::instance().percents_to_commanded(target_mixing_valve);

    SPDLOG_INFO(
            "TemperatureControlLoop: new valve position is {:.1f}% ({:0.04f}), temperature difference was "
            "{:+.3f}",
            target_mixing_valve, target_v, diff);

    IFPGA::get().setMixingValvePosition(target_v);

    return Settings::Setpoint::instance().timestep * 1000.0;
}
