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

#include "SubscriberThread.h"
#include <unistd.h>
#include "../MainSystem/Commands.h"
#include "../Utility/Logger.h"

/******************************************************************************
 * Note:
 * This macro is only used to reduce the number of lines of code this file
 * would be (and therefore reduce copy paste errors).
 * When calling this MACRO make sure the following preconditions are met:
 * 1. A SAL_MTM1M3TS named mtm1m3ts is defined.
 * 2. A Controller named controller is defined.
 * 3. Commands.h is imported
 *****************************************************************************/
#define ACCEPT_SAL_COMMAND(smallName, bigName) \
{ \
    MTM1M3TS_command_ ## smallName ## C data; \
    int commandId = this->mtm1m3ts.acceptCommand_## smallName(&data); \
    if (commandId > 0) { \
        controller.lock(); \
        controller.enqueue(new bigName ## Command(mtm1m3ts, commandId, data)); \
        controller.unlock(); \
    } \
}

namespace LSST {
namespace TS {
namespace MTM1M3TS {

SubscriberThread::SubscriberThread(SAL_MTM1M3TS& mtm1m3ts, Controller& controller) :
    mtm1m3ts(mtm1m3ts),
    controller(controller) {
    this->keepRunning = false;
}

void SubscriberThread::run() {
    Log.Info("Subscriber: Started");
    this->keepRunning = true; 
    while(this->keepRunning) {
        ACCEPT_SAL_COMMAND(abort, Abort)
        ACCEPT_SAL_COMMAND(applySetpoint, ApplySetpoint)
        ACCEPT_SAL_COMMAND(disable, Disable)
        ACCEPT_SAL_COMMAND(enable, Enable)
        ACCEPT_SAL_COMMAND(enterControl, EnterControl)
        ACCEPT_SAL_COMMAND(enterEngineering, EnterEngineering)
        ACCEPT_SAL_COMMAND(exitControl, ExitControl)
        ACCEPT_SAL_COMMAND(exitEngineering, ExitEngineering)
        ACCEPT_SAL_COMMAND(setFanPWM, SetFanPWM)
        ACCEPT_SAL_COMMAND(setHeaterPWM, SetHeaterPWM)
        ACCEPT_SAL_COMMAND(setLogLevel, SetLogLevel)
        ACCEPT_SAL_COMMAND(setMixingValve, SetMixingValve)
        ACCEPT_SAL_COMMAND(setSimulationMode, SetSimulationMode)
        ACCEPT_SAL_COMMAND(setValue, SetValue)
        ACCEPT_SAL_COMMAND(setVFD, SetVFD)
        ACCEPT_SAL_COMMAND(standby, Standby)
        ACCEPT_SAL_COMMAND(start, Start)
        usleep(1000);
    }
    Log.Info("Subscriber: Completed");
}

void SubscriberThread::stop() {
    Log.Info("Subscriber: Stop");
    this->keepRunning = false;
}

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */
