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

#include "Commands.h"
#include "MainSystem.h"
#include "../Utility/Logger.h"

/******************************************************************************
 * Note:
 * This macro is only used to reduce the number of lines of code this file
 * would be (and therefore reduce copy paste errors).
 * When calling this MACRO make sure the following preconditions are met:
 * 1. A class called SALCommand<> is defined.
 * 2. SAL_MTM1M3TS.h is imported
 *****************************************************************************/
#define DEFINE_SAL_COMMAND_IMPL(smallName, bigName) \
bigName ## Command:: bigName ## Command(SAL_MTM1M3TS& mtm1m3ts, int32_t commandID, MTM1M3TS_command_ ## smallName ## C data) : \
    Command(#bigName), \
    mtm1m3ts(mtm1m3ts) { \
    this->commandID = commandID; \
    this->data = data; \
} \
int32_t bigName ## Command::getCommandID() { return this->commandID; } \
MTM1M3TS_command_ ## smallName ## C& bigName ## Command::getData() { return this->data; } \
void bigName ## Command::execute(MainSystem& mainSystem) { mainSystem.smallName(*this); } \
void bigName ## Command::ack(int32_t ackCode, int32_t errorCode, std::string description) { \
    Command::ack(ackCode, errorCode, description); \
	this->mtm1m3ts.ackCommand_ ## smallName(this->commandID, ackCode, errorCode, (char*)description.c_str()); \
}

namespace LSST {
namespace TS {
namespace MTM1M3TS {

/******************************************************************************
 * Command
 *****************************************************************************/

Command::Command(std::string commandName) {
    this->commandName = commandName;
}

Command::~Command() { }

std::string Command::getName() { return this->commandName; }

void Command::execute(MainSystem& mainSystem) { }

void Command::ackInProgress() { this->ack(301, 0, "In-Progress"); }
void Command::ackComplete() { this->ack(303, 0, "Complete"); }
void Command::ackNotPermitted(int32_t errorCode, std::string description) { this->ack(-300, errorCode, "Not Permitted: " + description); }
void Command::ackAborted(int32_t errorCode, std::string description) { this->ack(-303, errorCode, "Aborted: " + description); }
void Command::ackFailed(int32_t errorCode, std::string description) { this->ack(-302, errorCode, "Failed: " + description); }
void Command::ackInvalidState(std::string description) { this->ackFailed(-320, description); }
void Command::ackInvalidParameter(std::string description) { this->ackFailed(-321, description); }
void Command::ackAlreadyInProgress(std::string description) { this->ackFailed(-322, description); }
void Command::ackExecutionBlocked(std::string description) { this->ackFailed(-323, description); }
void Command::ackAlreadyInState(std::string description) { this->ackFailed(-324, description); }

void Command::ack(int32_t ackCode, int32_t errorCode, std::string description) {
    if (ackCode < 0) {
        Log.Error("Main: Command %s has %s", this->commandName.c_str(), description.c_str());
    }
    else if (ackCode != 301) {
        Log.Info("Main: Command %s has %s", this->commandName.c_str(), description.c_str());
    }
    else {
        Log.Trace("Main: Command %s has %s", this->commandName.c_str(), description.c_str());
    }
}

/******************************************************************************
 * Update Command
 *****************************************************************************/

UpdateCommand::UpdateCommand() : Command("Update") { }
void UpdateCommand::execute(MainSystem& mainSystem) { mainSystem.update(*this); }
void UpdateCommand::ack(int32_t ackCode, int32_t errorCode, std::string description) { }

/******************************************************************************
 * Boot Commands
 *****************************************************************************/

BootCommand::BootCommand() : Command("Boot") { }
void BootCommand::execute(MainSystem& mainSystem) { mainSystem.boot(*this); }

/******************************************************************************
 * SAL Commands
 *****************************************************************************/

DEFINE_SAL_COMMAND_IMPL(abort, Abort)
DEFINE_SAL_COMMAND_IMPL(applySetpoint, ApplySetpoint)
DEFINE_SAL_COMMAND_IMPL(disable, Disable)
DEFINE_SAL_COMMAND_IMPL(enable, Enable)
DEFINE_SAL_COMMAND_IMPL(enterControl, EnterControl)
DEFINE_SAL_COMMAND_IMPL(enterEngineering, EnterEngineering)
DEFINE_SAL_COMMAND_IMPL(exitControl, ExitControl)
DEFINE_SAL_COMMAND_IMPL(exitEngineering, ExitEngineering)
DEFINE_SAL_COMMAND_IMPL(setFanPWM, SetFanPWM)
DEFINE_SAL_COMMAND_IMPL(setHeaterPWM, SetHeaterPWM)
DEFINE_SAL_COMMAND_IMPL(setLogLevel, SetLogLevel)
DEFINE_SAL_COMMAND_IMPL(setMixingValve, SetMixingValve)
DEFINE_SAL_COMMAND_IMPL(setSimulationMode, SetSimulationMode)
DEFINE_SAL_COMMAND_IMPL(setValue, SetValue)
DEFINE_SAL_COMMAND_IMPL(setVFD, SetVFD)
DEFINE_SAL_COMMAND_IMPL(standby, Standby)
DEFINE_SAL_COMMAND_IMPL(start, Start)

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */
