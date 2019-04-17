/******************************************************************************
 * This file is part of MTM1M3TS.
 *
 * Developed for the LSST Telescope and Site Systems.
 * This product includes software developed by the LSST Project
 * (https://www.lsst.org).
 * See the COPYRIGHT file at the top-level directory of this distribution
 * for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "ControllerThread.h"
#include "../Domain/Result.h"
#include "../MainSystem/Controller.h"
#include "../Utility/Logger.h"
#include <unistd.h>

namespace LSST {
namespace TS {
namespace MTM1M3TS {

ControllerThread::ControllerThread(Controller& controller) 
    : controller(controller) { }

void ControllerThread::run() {
	Log.Info("Controller: Started");
    this->keepRunning = true;
	while(this->keepRunning) {
		this->controller.lock();
		Result<Command*> command = this->controller.dequeue();
		this->controller.unlock();
		if (command.Valid) {
			this->controller.execute(command.Value);
		}
		else {
			usleep(1000);
		}
	}
	Log.Info("Controller: Completed");
}

void ControllerThread::stop() {
	Log.Info("Controller: Stop");
	this->keepRunning = false;
}

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */
