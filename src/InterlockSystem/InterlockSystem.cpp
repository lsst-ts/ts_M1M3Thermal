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

#include "InterlockSystem.h"
#include "../Utility/Timestamp.h"

namespace LSST {
namespace TS {
namespace MTM1M3TS {

/******************************************************************************
 * Interlock System Model
 *****************************************************************************/

InterlockSystemModel::InterlockSystemModel(InterlockSystemSettings& settings, InterlockSystemData& data, ThermalFPGASystemData& thermalFPGASystemData) :
	settings(settings),
	data(data),
	thermalFPGASystemData(thermalFPGASystemData) {
	this->heartbeatToggleCount = this->settings.HeartbeatTogglePeriod;
}

void InterlockSystemModel::update() {
	--this->heartbeatToggleCount;
	if (this->heartbeatToggleCount == 0) {
		this->data.HeartbeatCommand = !this->data.HeartbeatCommand;
		this->heartbeatToggleCount = this->settings.HeartbeatTogglePeriod;
	}

	this->data.SampleTimestamp = Timestamp::fromFPGA(this->thermalFPGASystemData.DigitalInputSampleTimestamp);
	this->data.RawSample = this->thermalFPGASystemData.DigitalInputStates;
	this->data.FanCoilHeatersOffInterlock = this->settings.RawSampleToFanCoilHeatersOffInterlock.evaluate(this->data.RawSample);
	this->data.CoolantPumpOffInterlock = this->settings.RawSampleToCoolantPumpOffInterlock.evaluate(this->data.RawSample);
	this->data.GISHeartbeatLostInterlock = this->settings.RawSampleToGISHeartbeatLostInterlock.evaluate(this->data.RawSample);
	this->data.MixingValveClosedInterlock = this->settings.RawSampleToMixingValveClosedInterlock.evaluate(this->data.RawSample);
	this->data.SupportSystemHeartbeatLostInterlock = this->settings.RawSampleToSupportSystemHeartbeatLostInterlock.evaluate(this->data.RawSample);
	this->data.CellDoorOpenInterlock = this->settings.RawSampleToCellDoorOpenInterlock.evaluate(this->data.RawSample);
	this->data.GISEarthquakeInterlock = this->settings.RawSampleToGISEarthquakeInterlock.evaluate(this->data.RawSample);
	this->data.CoolantPumpEStopInterlock = this->settings.RawSampleToCoolantPumpEStopInterlock.evaluate(this->data.RawSample);
	this->data.CabinetOverTempInterlock = this->settings.RawSampleToCabinetOverTempInterlock.evaluate(this->data.RawSample);
}

/******************************************************************************
 * Interlock System State
 *****************************************************************************/

InterlockSystemState::InterlockSystemState() { 
	this->stateName = "BaseState";
}

InterlockSystemState::~InterlockSystemState() { }

Result<InterlockSystemStates> InterlockSystemState::invalidState(std::string command) {
	return Result<InterlockSystemStates>(InterlockSystemStates::NoStateTransitionInvalidState, false, "The interlock system cannot execute the " + command + " command from the " + this->stateName + " state.");
}

Result<InterlockSystemStates> InterlockSystemState::update(InterlockSystemModel& model) { return this->invalidState("update"); }

Result<InterlockSystemStates> InterlockSystemState::start(InterlockSystemModel& model) { return this->invalidState("start"); }
Result<InterlockSystemStates> InterlockSystemState::standby(InterlockSystemModel& model) { return this->invalidState("standby");  }

/******************************************************************************
 * Standby Interlock System State
 *****************************************************************************/

StandbyInterlockSystemState::StandbyInterlockSystemState() {
	this->stateName = "Standby";
 }

Result<InterlockSystemStates> StandbyInterlockSystemState::update(InterlockSystemModel& model) {
	InterlockSystemSettings& settings = model.getSettings();
	InterlockSystemData& data = model.getData();

	model.update();

	data.FanCoilHeatersOffInterlockLimit = settings.FanCoilHeatersOffInterlockLimit.evaluate(data.FanCoilHeatersOffInterlock);
	data.CoolantPumpOffInterlockLimit = settings.CoolantPumpOffInterlockLimit.evaluate(data.CoolantPumpOffInterlock);
	data.GISHeartbeatLostInterlockLimit = settings.GISHeartbeatLostInterlockLimit.evaluate(data.GISHeartbeatLostInterlock);
	data.MixingValveClosedInterlockLimit = settings.MixingValveClosedInterlockLimit.evaluate(data.MixingValveClosedInterlock);
	data.SupportSystemHeartbeatLostInterlockLimit = settings.SupportSystemHeartbeatLostInterlockLimit.evaluate(data.SupportSystemHeartbeatLostInterlock);
	data.CellDoorOpenInterlockLimit = settings.CellDoorOpenInterlockLimit.evaluate(data.CellDoorOpenInterlock);
	data.GISEarthquakeInterlockLimit = settings.GISEarthquakeInterlockLimit.evaluate(data.GISEarthquakeInterlock);
	data.CoolantPumpEStopInterlockLimit = settings.CoolantPumpEStopInterlockLimit.evaluate(data.CoolantPumpEStopInterlock);
	data.CabinetOverTempInterlockLimit = settings.CabinetOverTempInterlockLimit.evaluate(data.CabinetOverTempInterlock);

	return Result<InterlockSystemStates>(InterlockSystemStates::NoStateTransition);
}

Result<InterlockSystemStates> StandbyInterlockSystemState::start(InterlockSystemModel& model) {
	return Result<InterlockSystemStates>(InterlockSystemStates::Disabled);
}

/******************************************************************************
 * Disabled Interlock System State
 *****************************************************************************/

DisabledInterlockSystemState::DisabledInterlockSystemState() { 
	this->stateName = "Disabled";
}

Result<InterlockSystemStates> DisabledInterlockSystemState::update(InterlockSystemModel& model) {
	InterlockSystemStates nextState = InterlockSystemStates::NoStateTransition;
	InterlockSystemSettings& settings = model.getSettings();
	InterlockSystemData& data = model.getData();

	model.update();

	data.FanCoilHeatersOffInterlockLimit = settings.FanCoilHeatersOffInterlockLimit.evaluate(data.FanCoilHeatersOffInterlock);
	data.CoolantPumpOffInterlockLimit = settings.CoolantPumpOffInterlockLimit.evaluate(data.CoolantPumpOffInterlock);
	data.GISHeartbeatLostInterlockLimit = settings.GISHeartbeatLostInterlockLimit.evaluate(data.GISHeartbeatLostInterlock);
	data.MixingValveClosedInterlockLimit = settings.MixingValveClosedInterlockLimit.evaluate(data.MixingValveClosedInterlock);
	data.SupportSystemHeartbeatLostInterlockLimit = settings.SupportSystemHeartbeatLostInterlockLimit.evaluate(data.SupportSystemHeartbeatLostInterlock);
	data.CellDoorOpenInterlockLimit = settings.CellDoorOpenInterlockLimit.evaluate(data.CellDoorOpenInterlock);
	data.GISEarthquakeInterlockLimit = settings.GISEarthquakeInterlockLimit.evaluate(data.GISEarthquakeInterlock);
	data.CoolantPumpEStopInterlockLimit = settings.CoolantPumpEStopInterlockLimit.evaluate(data.CoolantPumpEStopInterlock);
	data.CabinetOverTempInterlockLimit = settings.CabinetOverTempInterlockLimit.evaluate(data.CabinetOverTempInterlock);

	Limits limit = mergeLimits(data.FanCoilHeatersOffInterlockLimit, data.CoolantPumpOffInterlockLimit);
	limit = mergeLimits(limit, data.GISHeartbeatLostInterlockLimit);
	limit = mergeLimits(limit, data.MixingValveClosedInterlockLimit);
	limit = mergeLimits(limit, data.SupportSystemHeartbeatLostInterlockLimit);
	limit = mergeLimits(limit, data.CellDoorOpenInterlockLimit);
	limit = mergeLimits(limit, data.GISEarthquakeInterlockLimit);
	limit = mergeLimits(limit, data.CoolantPumpEStopInterlockLimit);
	limit = mergeLimits(limit, data.CabinetOverTempInterlockLimit);
	
	switch(limit) {
	case Limits::UNKNOWN: nextState = InterlockSystemStates::NoStateTransition; break;
	case Limits::OK: nextState = InterlockSystemStates::NoStateTransition; break;
	case Limits::WARNING: nextState = InterlockSystemStates::NoStateTransition; break;
	case Limits::FAULT: nextState = InterlockSystemStates::Fault; break;
	case Limits::BYPASSED_OK: nextState = InterlockSystemStates::NoStateTransition; break;
	case Limits::BYPASSED_WARNING: nextState = InterlockSystemStates::NoStateTransition; break;
	case Limits::BYPASSED_FAULT: nextState = InterlockSystemStates::NoStateTransition; break;
	}

 	return Result<InterlockSystemStates>(nextState);
}

Result<InterlockSystemStates> DisabledInterlockSystemState::standby(InterlockSystemModel& model) {
	return Result<InterlockSystemStates>(InterlockSystemStates::Standby);
}

/******************************************************************************
 * Fault Interlock System State
 *****************************************************************************/

FaultInterlockSystemState::FaultInterlockSystemState() {
	this->stateName = "Fault";
}

Result<InterlockSystemStates> FaultInterlockSystemState::update(InterlockSystemModel& model) {
	InterlockSystemSettings& settings = model.getSettings();
	InterlockSystemData& data = model.getData();

	model.update();

	data.FanCoilHeatersOffInterlockLimit = settings.FanCoilHeatersOffInterlockLimit.evaluate(data.FanCoilHeatersOffInterlock);
	data.CoolantPumpOffInterlockLimit = settings.CoolantPumpOffInterlockLimit.evaluate(data.CoolantPumpOffInterlock);
	data.GISHeartbeatLostInterlockLimit = settings.GISHeartbeatLostInterlockLimit.evaluate(data.GISHeartbeatLostInterlock);
	data.MixingValveClosedInterlockLimit = settings.MixingValveClosedInterlockLimit.evaluate(data.MixingValveClosedInterlock);
	data.SupportSystemHeartbeatLostInterlockLimit = settings.SupportSystemHeartbeatLostInterlockLimit.evaluate(data.SupportSystemHeartbeatLostInterlock);
	data.CellDoorOpenInterlockLimit = settings.CellDoorOpenInterlockLimit.evaluate(data.CellDoorOpenInterlock);
	data.GISEarthquakeInterlockLimit = settings.GISEarthquakeInterlockLimit.evaluate(data.GISEarthquakeInterlock);
	data.CoolantPumpEStopInterlockLimit = settings.CoolantPumpEStopInterlockLimit.evaluate(data.CoolantPumpEStopInterlock);
	data.CabinetOverTempInterlockLimit = settings.CabinetOverTempInterlockLimit.evaluate(data.CabinetOverTempInterlock);

 	return Result<InterlockSystemStates>(InterlockSystemStates::NoStateTransition);
}

Result<InterlockSystemStates> FaultInterlockSystemState::standby(InterlockSystemModel& model) {
	return Result<InterlockSystemStates>(InterlockSystemStates::Standby);
}

/******************************************************************************
 * Interlock System Context
 *****************************************************************************/

InterlockSystemContext::InterlockSystemContext(InterlockSystemModel& model) :
	model(model) {
	this->currentState = 0;
	this->updateState(InterlockSystemStates::Standby);
}

Result<SubsystemResults> InterlockSystemContext::update() { return this->updateState(this->currentState->update(this->model)); }

Result<SubsystemResults> InterlockSystemContext::start() { return this->updateState(this->currentState->start(this->model)); }
Result<SubsystemResults> InterlockSystemContext::standby() { return this->updateState(this->currentState->standby(this->model)); }

Result<SubsystemResults> InterlockSystemContext::updateState(Result<InterlockSystemStates> actionResult) {
	InterlockSystemStates state = actionResult.Value;
	
	Result<SubsystemResults> result;

	if (state >= InterlockSystemStates::Standby) {
		this->model.getData().State = state;
	}

	switch(state) {
	case InterlockSystemStates::NoStateTransition: 
		break;
	case InterlockSystemStates::NoStateTransitionInvalidState: 
		result = Result<SubsystemResults>(SubsystemResults::InvalidState, false, actionResult.Description);
		break;
	case InterlockSystemStates::NoStateTransitionInvalidParameter: 
		result = Result<SubsystemResults>(SubsystemResults::InvalidParameter, false, actionResult.Description);
		break;
	case InterlockSystemStates::Standby:
		this->currentState = &this->standbyState;
		break;
	case InterlockSystemStates::Disabled:
		this->currentState = &this->disabledState;
		break;
	case InterlockSystemStates::Fault: 
		this->currentState = &this->faultState;
		result = Result<SubsystemResults>(SubsystemResults::Fault, true, actionResult.Description);
		break;
	}

	return result;
}

/******************************************************************************
 * Interlock System
 *****************************************************************************/

InterlockSystem::InterlockSystem(InterlockSystemContext& context) :
	context(context) {
}

Result<SubsystemResults> InterlockSystem::update() { return this->context.update(); }

Result<SubsystemResults> InterlockSystem::start() { return this->context.start(); }
Result<SubsystemResults> InterlockSystem::standby() { return this->context.standby(); }

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */
