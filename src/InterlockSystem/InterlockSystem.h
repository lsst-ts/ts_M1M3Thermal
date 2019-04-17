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

#ifndef INTERLOCKSYSTEM_INTERLOCKSYSTEM_H_
#define INTERLOCKSYSTEM_INTERLOCKSYSTEM_H_

#include "../Domain/Domain.h"
#include "../Domain/Result.h"

namespace LSST {
namespace TS {
namespace MTM1M3TS {

/******************************************************************************
 * Interlock System Model
 *****************************************************************************/

class InterlockSystemModel {
private:
	InterlockSystemSettings& settings;
	InterlockSystemData& data;
	ThermalFPGASystemData& thermalFPGASystemData;
	
	uint32_t heartbeatToggleCount;

public:
	InterlockSystemModel(InterlockSystemSettings& settings, InterlockSystemData& data, ThermalFPGASystemData& thermalFPGASystemData);

	inline InterlockSystemSettings& getSettings() { return this->settings; }
	inline InterlockSystemData& getData() { return this->data; }

	void update();
};

/******************************************************************************
 * Interlock System State
 *****************************************************************************/

class InterlockSystemState {
protected:
	std::string stateName;

	Result<InterlockSystemStates> invalidState(std::string command);

public:
	InterlockSystemState();
	virtual ~InterlockSystemState();

	virtual Result<InterlockSystemStates> update(InterlockSystemModel& model);

	virtual Result<InterlockSystemStates> start(InterlockSystemModel& model);
	virtual Result<InterlockSystemStates> standby(InterlockSystemModel& model);
};

/******************************************************************************
 * Standby Interlock System State
 *****************************************************************************/

class StandbyInterlockSystemState : public InterlockSystemState {
public:
	StandbyInterlockSystemState();

	Result<InterlockSystemStates> update(InterlockSystemModel& model);

	Result<InterlockSystemStates> start(InterlockSystemModel& model);
};

/******************************************************************************
 * Disabled Interlock System State
 *****************************************************************************/

class DisabledInterlockSystemState : public InterlockSystemState {
public:
	DisabledInterlockSystemState();

	Result<InterlockSystemStates> update(InterlockSystemModel& model);

	Result<InterlockSystemStates> standby(InterlockSystemModel& model);
};

/******************************************************************************
 * Fault Interlock System State
 *****************************************************************************/

class FaultInterlockSystemState : public InterlockSystemState {
public:
	FaultInterlockSystemState();

	Result<InterlockSystemStates> update(InterlockSystemModel& model);

	Result<InterlockSystemStates> standby(InterlockSystemModel& model);
};

/******************************************************************************
 * Interlock System Context
 *****************************************************************************/

class InterlockSystemContext {
private:
	InterlockSystemModel& model;

	InterlockSystemState* currentState;
	StandbyInterlockSystemState standbyState;
	DisabledInterlockSystemState disabledState;
	FaultInterlockSystemState faultState;

public:
	InterlockSystemContext(InterlockSystemModel& model);

	Result<SubsystemResults> update();

	Result<SubsystemResults> start();
	Result<SubsystemResults> standby();

private:
	Result<SubsystemResults> updateState(Result<InterlockSystemStates> actionResult);
};

/******************************************************************************
 * Interlock System
 *****************************************************************************/

class InterlockSystem {
private:
	InterlockSystemContext& context;

public:
	InterlockSystem(InterlockSystemContext& context);

	Result<SubsystemResults> update();

	Result<SubsystemResults> start();
	Result<SubsystemResults> standby();
};

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */

#endif /* INTERLOCKSYSTEM_INTERLOCKSYSTEM_H_ */
