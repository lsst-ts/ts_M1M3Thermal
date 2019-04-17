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

#include "Controller.h"
#include "Commands.h"
#include "../Utility/Logger.h"

namespace LSST {
namespace TS {
namespace MTM1M3TS {

Controller::Controller(MainSystem& mainSystem)
	: mainSystem(mainSystem) {
	Log.Debug("Controller: Controller()");
	pthread_mutex_init(&this->mutex, NULL);
}

Controller::~Controller() {
	this->clear();
	pthread_mutex_destroy(&this->mutex);
}

void Controller::lock() {
	Log.Trace("Controller: lock()");
	pthread_mutex_lock(&this->mutex);
}

void Controller::unlock() {
	Log.Trace("Controller: unlock()");
	pthread_mutex_unlock(&this->mutex);
}

void Controller::clear() {
	Log.Trace("Controller: clear()");
	this->lock();
	while(!this->queue.empty()) {
		Result<Command*> command = this->dequeue();
		if (command.Valid) {
			delete command.Value;
		}
	}
	this->unlock();
}

void Controller::enqueue(Command* command) {
	Log.Trace("Controller: enqueue()");
	this->queue.push(command);
}

Result<Command*> Controller::dequeue() {
	Result<Command*> result = Result<Command*>(0, false);
	Log.Trace("Controller: dequeue()");
	if (!this->queue.empty()) {
		result.Value = this->queue.front();
		result.Valid = true;
		this->queue.pop();
	}
	return result;
}

void Controller::execute(Command* command) {
	Log.Trace("Controller: execute(%s)", command->getName().c_str());
	command->ackInProgress();
	command->execute(mainSystem);
	delete command;
}

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */
