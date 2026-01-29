/*
 * Tasks controler, managing other tasks.
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

#include <cRIO/ControllerThread.h>

#include "Events/AppliedSetpoints.h"
#include "Settings/Setpoint.h"
#include "Tasks/Controller.h"

using namespace LSST::M1M3::TS::Tasks;

Controller::Controller(token) {}

void Controller::set_setpoints(float glycol, float heaters, bool save) {
    const std::lock_guard<std::mutex> lock(_lock);

    auto small_change = Events::AppliedSetpoints::instance().set_applied_setpoints(glycol, heaters);

    if (small_change.first) {
        if (_glycol_temperature_task == nullptr) {
            _glycol_temperature_task = std::make_shared<GlycolTemperatureControl>();
            cRIO::ControllerThread::instance().enqueue(_glycol_temperature_task);
        }
    } else {
        if (_glycol_temperature_task != nullptr) {
            cRIO::ControllerThread::instance().remove(_glycol_temperature_task);
            SPDLOG_INFO("EGW's control PID reset.");
        }

        _glycol_temperature_task = std::make_shared<GlycolTemperatureControl>();
        cRIO::ControllerThread::instance().enqueue(_glycol_temperature_task);
    }

    if (small_change.second) {
        if (_heaters_temperature_task == nullptr) {
            _heaters_temperature_task = std::make_shared<HeatersTemperatureControl>();
            cRIO::ControllerThread::instance().enqueue(_heaters_temperature_task);
        }
    } else {
        if (_heaters_temperature_task != nullptr) {
            cRIO::ControllerThread::instance().remove(_heaters_temperature_task);
            SPDLOG_INFO("Heaters control PID reset.");
        }

        _heaters_temperature_task = std::make_shared<HeatersTemperatureControl>();
        cRIO::ControllerThread::instance().enqueue(_heaters_temperature_task);
    }

    Events::AppliedSetpoints::instance().send();
    SPDLOG_INFO("Glycol setpoints: {:0.2f} FCU heaters setpoint: {:0.2f}", glycol, heaters);
    if (save == true) {
        Settings::Setpoint::instance().save_setpoints(glycol, heaters);
    }
}
