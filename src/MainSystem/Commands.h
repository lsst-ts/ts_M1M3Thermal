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

#ifndef MAINSYSTEM_COMMANDS_H_
#define MAINSYSTEM_COMMANDS_H_

#include <SAL_MTM1M3TS.h>

/******************************************************************************
 * Note:
 * This macro is only used to reduce the number of lines of code this file
 * would be (and therefore reduce copy paste errors).
 * When calling this MACRO make sure the following preconditions are met:
 * 1. A class called SALCommand<> is defined.
 * 2. SAL_MTM1M3TS.h is imported
 *****************************************************************************/
#define DEFINE_SAL_COMMAND_CLASS(smallName, bigName) \
class bigName ## Command : public Command { \
private: \
	SAL_MTM1M3TS& mtm1m3ts; \
	int32_t commandID; \
	MTM1M3TS_command_ ## smallName ## C data; \
 \
public: \
	bigName ## Command(SAL_MTM1M3TS& mtm1m3ts, int32_t commandID, MTM1M3TS_command_ ## smallName ## C data); \
 \
	int32_t getCommandID(); \
	MTM1M3TS_command_ ## smallName ## C& getData(); \
 \
	void execute(MainSystem& mainSystem); \
	void ack(int32_t ackCode, int32_t errorCode, std::string description); \
};

namespace LSST {
namespace TS {
namespace MTM1M3TS {

class MainSystem;

/******************************************************************************
 * Command
 *****************************************************************************/

class Command {
private:
	std::string commandName;

public:
	Command(std::string commandName = "Undefined");
	virtual ~Command();

	std::string getName();

	virtual void execute(MainSystem& mainSystem);

	void ackInProgress();
	void ackComplete();
	void ackNotPermitted(int32_t errorCode, std::string description);
	void ackAborted(int32_t errorCode, std::string description);
	void ackFailed(int32_t errorCode, std::string description);
	void ackInvalidState(std::string description);
	void ackInvalidParameter(std::string description);
	void ackAlreadyInProgress(std::string description);
	void ackExecutionBlocked(std::string description);
	void ackAlreadyInState(std::string description);

	virtual void ack(int32_t ackCode, int32_t errorCode, std::string description);
};

/******************************************************************************
 * Update Command
 *****************************************************************************/

class UpdateCommand : public Command {
public:
	UpdateCommand();

	void execute(MainSystem& mainSystem);
	void ack(int32_t ackCode, int32_t errorCode, std::string description);
};

/******************************************************************************
 * Boot Commands
 *****************************************************************************/

class BootCommand : public Command {
public:
	BootCommand();

	void execute(MainSystem& mainSystem);
};

/******************************************************************************
 * SAL Commands
 *****************************************************************************/

DEFINE_SAL_COMMAND_CLASS(abort, Abort)
DEFINE_SAL_COMMAND_CLASS(applySetpoint, ApplySetpoint)
DEFINE_SAL_COMMAND_CLASS(disable, Disable)
DEFINE_SAL_COMMAND_CLASS(enable, Enable)
DEFINE_SAL_COMMAND_CLASS(enterControl, EnterControl)
DEFINE_SAL_COMMAND_CLASS(enterEngineering, EnterEngineering)
DEFINE_SAL_COMMAND_CLASS(exitControl, ExitControl)
DEFINE_SAL_COMMAND_CLASS(exitEngineering, ExitEngineering)
DEFINE_SAL_COMMAND_CLASS(setFanPWM, SetFanPWM)
DEFINE_SAL_COMMAND_CLASS(setHeaterPWM, SetHeaterPWM)
DEFINE_SAL_COMMAND_CLASS(setLogLevel, SetLogLevel)
DEFINE_SAL_COMMAND_CLASS(setMixingValve, SetMixingValve)
DEFINE_SAL_COMMAND_CLASS(setSimulationMode, SetSimulationMode)
DEFINE_SAL_COMMAND_CLASS(setValue, SetValue)
DEFINE_SAL_COMMAND_CLASS(setVFD, SetVFD)
DEFINE_SAL_COMMAND_CLASS(standby, Standby)
DEFINE_SAL_COMMAND_CLASS(start, Start)

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */

#endif /* MAINSYSTEM_COMMANDS_H_ */
