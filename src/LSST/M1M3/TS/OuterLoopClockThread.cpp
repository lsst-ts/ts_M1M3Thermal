/*
 * Thread to catch outer loop clock interrupts.
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

#include <chrono>
#include <memory>

#include <spdlog/spdlog.h>

#include <cRIO/ControllerThread.h>

#include "Commands/Update.h"
#include "Events/EngineeringMode.h"
#include "Events/Heartbeat.h"
#include "Events/SummaryState.h"
#include "OuterLoopClockThread.h"
#include "Tasks/GlycolTemperatureControl.h"

using namespace std::chrono_literals;
using namespace LSST::M1M3::TS;

void OuterLoopClockThread::run(std::unique_lock<std::mutex> &lock) {
    SPDLOG_INFO("OuterLoopClockThread: Run");

    std::shared_ptr<Tasks::GlycolTemperatureControl> _glycol_temperature_task;

    while (keepRunning) {
        runCondition.wait_for(lock, 500ms);
        if (Events::SummaryState::instance().active()) {
            cRIO::ControllerThread::instance().enqueue(std::make_shared<Commands::Update>());
        }
	if (Events::SummaryState::instance().enabled() == true && Events::EngineeringMode::instance().is_enabled() == false) {
		if (_glycol_temperature_task == nullptr) {
			_glycol_temperature_task = std::make_shared<Tasks::GlycolTemperatureControl>();
		        cRIO::ControllerThread::instance().enqueue(_glycol_temperature_task);
		}
	} else {
		if (_glycol_temperature_task != nullptr) {
  		     cRIO::ControllerThread::instance().remove(_glycol_temperature_task);
		     _glycol_temperature_task = nullptr;
		}

	}
        Events::Heartbeat::instance().tryToggle();
    }
    SPDLOG_INFO("OuterLoopClockThread: Completed");
}
