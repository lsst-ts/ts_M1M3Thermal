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

#ifndef MAINSYSTEM_MAINSYSTEM_H_
#define MAINSYSTEM_MAINSYSTEM_H_

#include "Commands.h"
#include "../Domain/Domain.h"
#include "../Domain/Result.h"
#include "../InterlockSystem/InterlockSystem.h"

namespace LSST {
namespace TS {
namespace MTM1M3TS {

/******************************************************************************
 * Main System Model
 *****************************************************************************/

class MainSystemModel {
private:
	MainSystemSettings& settings;
	MainSystemData& data;
	InterlockSystem& interlockSystem;

	pthread_mutex_t mutex;
	
public:
	MainSystemModel(MainSystemSettings& settings, MainSystemData& data, InterlockSystem& interlockSystem);
	~MainSystemModel();

	inline MainSystemSettings& getSettings() { return this->settings; }
	inline MainSystemData& getData() { return this->data; }
	inline InterlockSystem& getInterlockSystem() { return this->interlockSystem; }

	void startUpdate();
	void endUpdate();

	void shutdown();
	void waitForShutdown();
};

/******************************************************************************
 * Main System State
 *****************************************************************************/

class MainSystemState {
protected:
	std::string stateName;

	Result<MainSystemStates> invalidState(Command* command);

public:
	MainSystemState();
	virtual ~MainSystemState();

	virtual Result<MainSystemStates> update(UpdateCommand& command, MainSystemModel& model);
	virtual Result<MainSystemStates> boot(BootCommand& command, MainSystemModel& model);

	virtual Result<MainSystemStates> abort(AbortCommand& command, MainSystemModel& model);
	virtual Result<MainSystemStates> applySetpoint(ApplySetpointCommand& command, MainSystemModel& model);
	virtual Result<MainSystemStates> disable(DisableCommand& command, MainSystemModel& model);
	virtual Result<MainSystemStates> enable(EnableCommand& command, MainSystemModel& model);
	virtual Result<MainSystemStates> enterControl(EnterControlCommand& command, MainSystemModel& model);
	virtual Result<MainSystemStates> enterEngineering(EnterEngineeringCommand& command, MainSystemModel& model);
	virtual Result<MainSystemStates> exitControl(ExitControlCommand& command, MainSystemModel& model);
	virtual Result<MainSystemStates> exitEngineering(ExitEngineeringCommand& command, MainSystemModel& model);
	virtual Result<MainSystemStates> setFanPWM(SetFanPWMCommand& command, MainSystemModel& model);
	virtual Result<MainSystemStates> setHeaterPWM(SetHeaterPWMCommand& command, MainSystemModel& model);
	virtual Result<MainSystemStates> setLogLevel(SetLogLevelCommand& command, MainSystemModel& model);
	virtual Result<MainSystemStates> setMixingValve(SetMixingValveCommand& command, MainSystemModel& model);
	virtual Result<MainSystemStates> setSimulationMode(SetSimulationModeCommand& command, MainSystemModel& model);
	virtual Result<MainSystemStates> setValue(SetValueCommand& command, MainSystemModel& model);
	virtual Result<MainSystemStates> setVFD(SetVFDCommand& command, MainSystemModel& model);
	virtual Result<MainSystemStates> standby(StandbyCommand& command, MainSystemModel& model);
	virtual Result<MainSystemStates> start(StartCommand& command, MainSystemModel& model);
};

/******************************************************************************
 * Offline Main System State
 *****************************************************************************/

class OfflineMainSystemState : public MainSystemState {
public:
	OfflineMainSystemState();

	Result<MainSystemStates> boot(BootCommand& command, MainSystemModel& model);
};

/******************************************************************************
 * Standby Main System State
 *****************************************************************************/

class StandbyMainSystemState : public MainSystemState {
public:
	StandbyMainSystemState();

	Result<MainSystemStates> update(UpdateCommand& command, MainSystemModel& model);

	Result<MainSystemStates> start(StartCommand& command, MainSystemModel& model);
	Result<MainSystemStates> exitControl(ExitControlCommand& command, MainSystemModel& model);
};

/******************************************************************************
 * Disabled Main System State
 *****************************************************************************/

class DisabledMainSystemState : public MainSystemState {
public:
	DisabledMainSystemState();

	Result<MainSystemStates> update(UpdateCommand& command, MainSystemModel& model);

	Result<MainSystemStates> standby(StandbyCommand& command, MainSystemModel& model);
	Result<MainSystemStates> enable(EnableCommand& command, MainSystemModel& model);
};

/******************************************************************************
 * Enabled Main System State
 *****************************************************************************/

class EnabledMainSystemState : public MainSystemState {
public:
	EnabledMainSystemState();

	Result<MainSystemStates> update(UpdateCommand& command, MainSystemModel& model);

	Result<MainSystemStates> enterEngineering(EnterEngineeringCommand& command, MainSystemModel& model);
	Result<MainSystemStates> disable(DisableCommand& command, MainSystemModel& model);
};

/******************************************************************************
 * Engineering Main System State
 *****************************************************************************/

class EngineeringMainSystemState : public MainSystemState {
public:
	EngineeringMainSystemState();

	Result<MainSystemStates> update(UpdateCommand& command, MainSystemModel& model);

	Result<MainSystemStates> exitEngineering(ExitEngineeringCommand& command, MainSystemModel& model);
};

/******************************************************************************
 * Fault Main System State
 *****************************************************************************/

class FaultMainSystemState : public MainSystemState {
public:
	FaultMainSystemState();

	Result<MainSystemStates> update(UpdateCommand& command, MainSystemModel& model);

	Result<MainSystemStates> standby(StandbyCommand& command, MainSystemModel& model);
};

/******************************************************************************
 * Main System Context
 *****************************************************************************/

class MainSystemContext {
private:
	MainSystemModel& model;

	MainSystemState* currentState;
	OfflineMainSystemState offlineState;
	StandbyMainSystemState standbyState;
	DisabledMainSystemState disabledState;
	EnabledMainSystemState enabledState;
	EngineeringMainSystemState engineeringState;
	FaultMainSystemState faultState;

public:
	MainSystemContext(MainSystemModel& model);

	void update(UpdateCommand& command);
	void boot(BootCommand& command);

	void abort(AbortCommand& command);
	void applySetpoint(ApplySetpointCommand& command);
	void disable(DisableCommand& command);
	void enable(EnableCommand& command);
	void enterControl(EnterControlCommand& command);
	void enterEngineering(EnterEngineeringCommand& command);
	void exitControl(ExitControlCommand& command);
	void exitEngineering(ExitEngineeringCommand& command);
	void setFanPWM(SetFanPWMCommand& command);
	void setHeaterPWM(SetHeaterPWMCommand& command);
	void setLogLevel(SetLogLevelCommand& command);
	void setMixingValve(SetMixingValveCommand& command);
	void setSimulationMode(SetSimulationModeCommand& command);
	void setValue(SetValueCommand& command);
	void setVFD(SetVFDCommand& command);
	void standby(StandbyCommand& command);
	void start(StartCommand& command);

private:
	void updateState(Result<MainSystemStates> actionResult);
};

/******************************************************************************
 * Main System
 *****************************************************************************/

class MainSystem {
private:
	MainSystemContext& context;

public:
	MainSystem(MainSystemContext& context);

	void update(UpdateCommand& command);
	void boot(BootCommand& command);

	void abort(AbortCommand& command);
	void applySetpoint(ApplySetpointCommand& command);
	void disable(DisableCommand& command);
	void enable(EnableCommand& command);
	void enterControl(EnterControlCommand& command);
	void enterEngineering(EnterEngineeringCommand& command);
	void exitControl(ExitControlCommand& command);
	void exitEngineering(ExitEngineeringCommand& command);
	void setFanPWM(SetFanPWMCommand& command);
	void setHeaterPWM(SetHeaterPWMCommand& command);
	void setLogLevel(SetLogLevelCommand& command);
	void setMixingValve(SetMixingValveCommand& command);
	void setSimulationMode(SetSimulationModeCommand& command);
	void setValue(SetValueCommand& command);
	void setVFD(SetVFDCommand& command);
	void standby(StandbyCommand& command);
	void start(StartCommand& command);
};

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */

#endif /* MAINSYSTEM_MAINSYSTEM_H_ */
