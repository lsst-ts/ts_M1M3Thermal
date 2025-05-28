/*
 * Tasks controlling FCU heaters.
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

#include <cRIO/ThermalILC.h>

#include "Events/AppliedSetpoints.h"
#include "Events/FcuTargets.h"
#include "Settings/Heaters.h"
#include "Tasks/HeatersTemperatureControl.h"
#include "Telemetry/ThermalData.h"

using namespace LSST::M1M3::TS::Tasks;

HeatersTemperatureControl::HeatersTemperatureControl() {}

LSST::cRIO::task_return_t HeatersTemperatureControl::run() {
    auto heaterPWM = Events::FcuTargets::instance().get_heaterPWM();
    auto fanRPM = Events::FcuTargets::instance().get_fanRPM();
    auto temperature = Telemetry::ThermalData::instance().get_absoluteTemperature();
    auto target_temperature = Events::AppliedSetpoints::instance().get_applied_heaters_setpoint();

    auto& h_settings = Settings::Heaters::instance();

    std::vector<int> target_heater(LSST::cRIO::NUM_TS_ILC);
    std::vector<int> target_fan(LSST::cRIO::NUM_TS_ILC);
    for (int i = 0; i < LSST::cRIO::NUM_TS_ILC; i++) {
        assert(h_settings.heaters_PID[i] != nullptr);
        target_heater[i] = h_settings.heaters_PID[i]->process(target_temperature, temperature[i]);

        if (target_heater[i] > 255) {
            target_heater[i] = 255;
        }
        if (target_heater[i] < 0) {
            target_heater[i] = 0;
        }
        target_fan[i] = fanRPM[i] == 0 ? 20 : fanRPM[i] / 10;
    }
    try {
        Events::FcuTargets::instance().set_FCU_heaters_fans(target_heater, target_fan);
    } catch (std::exception& ex) {
        SPDLOG_WARN("Error executing FCU heaters task: {}", ex.what());
    }

    return h_settings.interval * 1000.0;
}
