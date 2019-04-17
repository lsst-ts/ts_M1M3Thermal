/******************************************************************************
 * This file is part of MTM1M3.
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

#include "MainSystem.h"
#include "../Utility/Logger.h"
#include "../Utility/Timestamp.h"

namespace LSST {
namespace TS {
namespace MTM1M3TS {

/******************************************************************************
 * Main System Model
 *****************************************************************************/

MainSystemModel::MainSystemModel(MainSystemSettings& settings, MainSystemData& data, InterlockSystem& interlockSystem) :
	settings(settings),
	data(data),
	interlockSystem(interlockSystem) {
	pthread_mutex_init(&this->mutex, NULL);
	pthread_mutex_lock(&this->mutex);
}

MainSystemModel::~MainSystemModel() {
	pthread_mutex_unlock(&this->mutex);
	pthread_mutex_destroy(&this->mutex);
}

void MainSystemModel::startUpdate() {

}

void MainSystemModel::endUpdate() {

}

void MainSystemModel::shutdown() {
	Log.Info("Main: Shutting Down");
	pthread_mutex_unlock(&this->mutex);
}

void MainSystemModel::waitForShutdown() {
	pthread_mutex_lock(&this->mutex);
	pthread_mutex_unlock(&this->mutex);
}

/******************************************************************************
 * Main System State
 *****************************************************************************/

MainSystemState::MainSystemState() { 
	this->stateName = "BaseState";
}

MainSystemState::~MainSystemState() { }

Result<MainSystemStates> MainSystemState::invalidState(Command* command) {
	command->ackInvalidState("Cannot execute the " + command->getName() + " command from the " + this->stateName + " state");
	return Result<MainSystemStates>(MainSystemStates::NoStateTransitionInvalidState, false, "The main system cannot execute the " + command->getName() + " command from the " + this->stateName + " state.");
}

Result<MainSystemStates> MainSystemState::update(UpdateCommand& command, MainSystemModel& model) { return this->invalidState(&command); }
Result<MainSystemStates> MainSystemState::boot(BootCommand& command, MainSystemModel& model) { return this->invalidState(&command); }

Result<MainSystemStates> MainSystemState::abort(AbortCommand& command, MainSystemModel& model) { return this->invalidState(&command); }
Result<MainSystemStates> MainSystemState::applySetpoint(ApplySetpointCommand& command, MainSystemModel& model) { return this->invalidState(&command); }
Result<MainSystemStates> MainSystemState::disable(DisableCommand& command, MainSystemModel& model) { return this->invalidState(&command); }
Result<MainSystemStates> MainSystemState::enable(EnableCommand& command, MainSystemModel& model) { return this->invalidState(&command); }
Result<MainSystemStates> MainSystemState::enterControl(EnterControlCommand& command, MainSystemModel& model) { return this->invalidState(&command); }
Result<MainSystemStates> MainSystemState::enterEngineering(EnterEngineeringCommand& command, MainSystemModel& model) { return this->invalidState(&command); }
Result<MainSystemStates> MainSystemState::exitControl(ExitControlCommand& command, MainSystemModel& model) { return this->invalidState(&command); }
Result<MainSystemStates> MainSystemState::exitEngineering(ExitEngineeringCommand& command, MainSystemModel& model) { return this->invalidState(&command); }
Result<MainSystemStates> MainSystemState::setFanPWM(SetFanPWMCommand& command, MainSystemModel& model) { return this->invalidState(&command); }
Result<MainSystemStates> MainSystemState::setHeaterPWM(SetHeaterPWMCommand& command, MainSystemModel& model) { return this->invalidState(&command); }

Result<MainSystemStates> MainSystemState::setLogLevel(SetLogLevelCommand& command, MainSystemModel& model) { 
	LoggerLevels level = LoggerLevels(command.getData().level);
	switch(level) {
	case LoggerLevels::Trace:
	case LoggerLevels::Debug:
	case LoggerLevels::Info:
	case LoggerLevels::Warn:
	case LoggerLevels::Error:
	case LoggerLevels::Fatal:
	case LoggerLevels::None:
		Log.SetLevel(level);
		command.ackComplete();
		return Result<MainSystemStates>(MainSystemStates::NoStateTransition);
	default:
		command.ackInvalidParameter("Invalid level parameter value.");
		return Result<MainSystemStates>(MainSystemStates::NoStateTransitionInvalidParameter);
	}
}

Result<MainSystemStates> MainSystemState::setMixingValve(SetMixingValveCommand& command, MainSystemModel& model) { return this->invalidState(&command); }
Result<MainSystemStates> MainSystemState::setSimulationMode(SetSimulationModeCommand& command, MainSystemModel& model) { return this->invalidState(&command); }
Result<MainSystemStates> MainSystemState::setValue(SetValueCommand& command, MainSystemModel& model) { return this->invalidState(&command); }
Result<MainSystemStates> MainSystemState::setVFD(SetVFDCommand& command, MainSystemModel& model) { return this->invalidState(&command); }
Result<MainSystemStates> MainSystemState::standby(StandbyCommand& command, MainSystemModel& model) { return this->invalidState(&command); }
Result<MainSystemStates> MainSystemState::start(StartCommand& command, MainSystemModel& model) { return this->invalidState(&command); }

/******************************************************************************
 * Offline Main System State
 *****************************************************************************/

OfflineMainSystemState::OfflineMainSystemState() {
	this->stateName = "Offline";
}

Result<MainSystemStates> OfflineMainSystemState::boot(BootCommand& command, MainSystemModel& model) {
	command.ackComplete();
	return Result<MainSystemStates>(MainSystemStates::Standby);
}

/********************************************
 * Standby Main System State
 *****************************************************************************/

StandbyMainSystemState::StandbyMainSystemState() {
	this->stateName = "Standby";
}

Result<MainSystemStates> StandbyMainSystemState::update(UpdateCommand& command, MainSystemModel& model) {
	model.startUpdate();
	model.getInterlockSystem().update();
	model.endUpdate();
	command.ackComplete();
	return Result<MainSystemStates>(MainSystemStates::NoStateTransition);
}

Result<MainSystemStates> StandbyMainSystemState::start(StartCommand& command, MainSystemModel& model) {
	model.getInterlockSystem().start();
	command.ackComplete();
	return Result<MainSystemStates>(MainSystemStates::Disabled);
}

Result<MainSystemStates> StandbyMainSystemState::exitControl(ExitControlCommand& command, MainSystemModel& model) {
	model.shutdown();
	command.ackComplete();
	return Result<MainSystemStates>(MainSystemStates::Offline);
}

/******************************************************************************
 * Disabled Main System State
 *****************************************************************************/

DisabledMainSystemState::DisabledMainSystemState() { 
	this->stateName = "Disabled";
}

Result<MainSystemStates> DisabledMainSystemState::update(UpdateCommand& command, MainSystemModel& model) {
	model.startUpdate();
	model.getInterlockSystem().update();
	model.endUpdate();
	command.ackComplete();
	return Result<MainSystemStates>(MainSystemStates::NoStateTransition);
}

Result<MainSystemStates> DisabledMainSystemState::standby(StandbyCommand& command, MainSystemModel& model) {
	model.getInterlockSystem().standby();
	command.ackComplete();
	return Result<MainSystemStates>(MainSystemStates::Standby);
}

Result<MainSystemStates> DisabledMainSystemState::enable(EnableCommand& command, MainSystemModel& model) {
	command.ackComplete();
	return Result<MainSystemStates>(MainSystemStates::Enabled);
}

/******************************************************************************
 * Enabled Main System State
 *****************************************************************************/

EnabledMainSystemState::EnabledMainSystemState() { 
	this->stateName = "Enabled";
}

Result<MainSystemStates> EnabledMainSystemState::update(UpdateCommand& command, MainSystemModel& model) {
	model.startUpdate();
	model.getInterlockSystem().update();
	model.endUpdate();
	command.ackComplete();
	return Result<MainSystemStates>(MainSystemStates::NoStateTransition);
}

Result<MainSystemStates> EnabledMainSystemState::enterEngineering(EnterEngineeringCommand& command, MainSystemModel& model) {
	command.ackComplete();
	return Result<MainSystemStates>(MainSystemStates::Engineering);
}

Result<MainSystemStates> EnabledMainSystemState::disable(DisableCommand& command, MainSystemModel& model) {
	command.ackComplete();
	return Result<MainSystemStates>(MainSystemStates::Disabled);
}

/******************************************************************************
 * Engineering Main System State
 *****************************************************************************/

EngineeringMainSystemState::EngineeringMainSystemState() { 
	this->stateName = "Engineering";
}

Result<MainSystemStates> EngineeringMainSystemState::update(UpdateCommand& command, MainSystemModel& model) {
	model.startUpdate();
	model.getInterlockSystem().update();
	model.endUpdate();
	command.ackComplete();
	return Result<MainSystemStates>(MainSystemStates::NoStateTransition);
}

Result<MainSystemStates> EngineeringMainSystemState::exitEngineering(ExitEngineeringCommand& command, MainSystemModel& model) {
	command.ackComplete();
	return Result<MainSystemStates>(MainSystemStates::Enabled);
}

/******************************************************************************
 * Fault Main System State
 *****************************************************************************/

FaultMainSystemState::FaultMainSystemState() {
	this->stateName = "Fault";
}

Result<MainSystemStates> FaultMainSystemState::update(UpdateCommand& command, MainSystemModel& model) {
	model.startUpdate();
	model.getInterlockSystem().update();
	model.endUpdate();
	command.ackComplete();
	return Result<MainSystemStates>(MainSystemStates::NoStateTransition);
}

Result<MainSystemStates> FaultMainSystemState::standby(StandbyCommand& command, MainSystemModel& model) {
	model.getInterlockSystem().standby();
	command.ackComplete();
	return Result<MainSystemStates>(MainSystemStates::Standby);
}

/******************************************************************************
 * Main System Context
 *****************************************************************************/

MainSystemContext::MainSystemContext(MainSystemModel& model) :
	model(model) {
	this->currentState = 0;
	this->updateState(MainSystemStates::Offline);
}

void MainSystemContext::update(UpdateCommand& command) { this->updateState(this->currentState->update(command, this->model)); }
void MainSystemContext::boot(BootCommand& command) { this->updateState(this->currentState->boot(command, this->model)); }

void MainSystemContext::abort(AbortCommand& command) { this->updateState(this->currentState->abort(command, this->model)); }
void MainSystemContext::applySetpoint(ApplySetpointCommand& command) { this->updateState(this->currentState->applySetpoint(command, this->model)); }
void MainSystemContext::disable(DisableCommand& command) { this->updateState(this->currentState->disable(command, this->model)); }
void MainSystemContext::enable(EnableCommand& command) { this->updateState(this->currentState->enable(command, this->model)); }
void MainSystemContext::enterControl(EnterControlCommand& command) { this->updateState(this->currentState->enterControl(command, this->model)); }
void MainSystemContext::enterEngineering(EnterEngineeringCommand& command) { this->updateState(this->currentState->enterEngineering(command, this->model)); }
void MainSystemContext::exitControl(ExitControlCommand& command) { this->updateState(this->currentState->exitControl(command, this->model)); }
void MainSystemContext::exitEngineering(ExitEngineeringCommand& command) { this->updateState(this->currentState->exitEngineering(command, this->model)); }
void MainSystemContext::setFanPWM(SetFanPWMCommand& command) { this->updateState(this->currentState->setFanPWM(command, this->model)); }
void MainSystemContext::setHeaterPWM(SetHeaterPWMCommand& command) { this->updateState(this->currentState->setHeaterPWM(command, this->model)); }
void MainSystemContext::setLogLevel(SetLogLevelCommand& command) { this->updateState(this->currentState->setLogLevel(command, this->model)); }
void MainSystemContext::setMixingValve(SetMixingValveCommand& command) { this->updateState(this->currentState->setMixingValve(command, this->model)); }
void MainSystemContext::setSimulationMode(SetSimulationModeCommand& command) { this->updateState(this->currentState->setSimulationMode(command, this->model)); }
void MainSystemContext::setValue(SetValueCommand& command) { this->updateState(this->currentState->setValue(command, this->model)); }
void MainSystemContext::setVFD(SetVFDCommand& command) { this->updateState(this->currentState->setVFD(command, this->model)); }
void MainSystemContext::standby(StandbyCommand& command) { this->updateState(this->currentState->standby(command, this->model)); }
void MainSystemContext::start(StartCommand& command) { this->updateState(this->currentState->start(command, this->model)); }

void MainSystemContext::updateState(Result<MainSystemStates> actionResult) {
	MainSystemStates state = actionResult.Value;

	if (state >= MainSystemStates::Offline) {
		this->model.getData().State = state;
	}

	switch(state) {
	case MainSystemStates::NoStateTransition: 
		break;
	case MainSystemStates::NoStateTransitionInvalidState:
		break;
	case MainSystemStates::NoStateTransitionInvalidParameter: 
		break;
	case MainSystemStates::Offline:
		Log.Info("MainSystem: Offline State");
		this->currentState = &this->offlineState;
		break;
	case MainSystemStates::Standby:
		Log.Info("MainSystem: Standby State");
		this->currentState = &this->standbyState;
		break;
	case MainSystemStates::Disabled:
		Log.Info("MainSystem: Disabled State");
		this->currentState = &this->disabledState;
		break;
	case MainSystemStates::Enabled:
		Log.Info("MainSystem: Enabled State");
		this->currentState = &this->enabledState;
		break;
	case MainSystemStates::Engineering:
		Log.Info("MainSystem: Engineering State");
		this->currentState = &this->engineeringState;
		break;
	case MainSystemStates::Fault:
		Log.Info("MainSystem: Fault State");
		this->currentState = &this->faultState;
		break;
	}
}

/******************************************************************************
 * Main System
 *****************************************************************************/

MainSystem::MainSystem(MainSystemContext& context) :
	context(context) {
}

void MainSystem::update(UpdateCommand& command) { this->context.update(command); }
void MainSystem::boot(BootCommand& command) { this->context.boot(command); }

void MainSystem::abort(AbortCommand& command) { this->context.abort(command); }
void MainSystem::applySetpoint(ApplySetpointCommand& command) { this->context.applySetpoint(command); }
void MainSystem::disable(DisableCommand& command) { this->context.disable(command); }
void MainSystem::enable(EnableCommand& command) { this->context.enable(command); }
void MainSystem::enterControl(EnterControlCommand& command) { this->context.enterControl(command); }
void MainSystem::enterEngineering(EnterEngineeringCommand& command) { this->context.enterEngineering(command); }
void MainSystem::exitControl(ExitControlCommand& command) { this->context.exitControl(command); }
void MainSystem::exitEngineering(ExitEngineeringCommand& command) { this->context.exitEngineering(command); }
void MainSystem::setFanPWM(SetFanPWMCommand& command) { this->context.setFanPWM(command); }
void MainSystem::setHeaterPWM(SetHeaterPWMCommand& command) { this->context.setHeaterPWM(command); }
void MainSystem::setLogLevel(SetLogLevelCommand& command) { this->context.setLogLevel(command); }
void MainSystem::setMixingValve(SetMixingValveCommand& command) { this->context.setMixingValve(command); }
void MainSystem::setSimulationMode(SetSimulationModeCommand& command) { this->context.setSimulationMode(command); }
void MainSystem::setValue(SetValueCommand& command) { this->context.setValue(command); }
void MainSystem::setVFD(SetVFDCommand& command) { this->context.setVFD(command); }
void MainSystem::standby(StandbyCommand& command) { this->context.standby(command); }
void MainSystem::start(StartCommand& command) { this->context.start(command); }

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */
