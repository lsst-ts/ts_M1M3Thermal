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

#ifndef UTILITY_MTM1M3TS_H_
#define UTILITY_MTM1M3TS_H_

#include <SAL_MTM1M3TSC.h>
#include <SAL_MTM1M3TS.h>

namespace LSST {
namespace TS {
namespace MTM1M3TS {

void initializeMTM1M3TS(SAL_MTM1M3TS& mtm1m3ts) {
    mtm1m3ts.setDebugLevel(0);
    mtm1m3ts.salProcessor((char*)"MTM1M3TS_command_abort");
    mtm1m3ts.salProcessor((char*)"MTM1M3TS_command_applySetpoint");
    mtm1m3ts.salProcessor((char*)"MTM1M3TS_command_disable");
    mtm1m3ts.salProcessor((char*)"MTM1M3TS_command_enable");
    mtm1m3ts.salProcessor((char*)"MTM1M3TS_command_enterControl");
    mtm1m3ts.salProcessor((char*)"MTM1M3TS_command_enterEngineering");
    mtm1m3ts.salProcessor((char*)"MTM1M3TS_command_exitControl");
    mtm1m3ts.salProcessor((char*)"MTM1M3TS_command_exitEngineering");
    mtm1m3ts.salProcessor((char*)"MTM1M3TS_command_setFanPWM");
    mtm1m3ts.salProcessor((char*)"MTM1M3TS_command_setHeaterPWM");
    mtm1m3ts.salProcessor((char*)"MTM1M3TS_command_setLogLevel");
    mtm1m3ts.salProcessor((char*)"MTM1M3TS_command_setMixingValve");
    mtm1m3ts.salProcessor((char*)"MTM1M3TS_command_setSimulationMode");
    mtm1m3ts.salProcessor((char*)"MTM1M3TS_command_setValue");
    mtm1m3ts.salProcessor((char*)"MTM1M3TS_command_setVFD");
    mtm1m3ts.salProcessor((char*)"MTM1M3TS_command_standby");
    mtm1m3ts.salProcessor((char*)"MTM1M3TS_command_start");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_appliedFanSpeed");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_appliedHeaterTemperature");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_appliedMixingValvePosition");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_appliedSetpoint");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_appliedSettingsMatchStart");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_detailedState");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_errorCode");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_flowSensorWarning");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_interlockWarning");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_logMessage");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_mixingValveWarning");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_powerSupplyWarning");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_settingVersions");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_simulationMode");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_summaryState");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_thermalActuatorILCInfo");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_thermalActuatorMainCalibrationInfo");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_thermalActuatorBackupCalibrationInfo");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_thermalActuatorPositionInfo");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_thermalActuatorState");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_thermalActuatorWarning");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_thermalWarning");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_vfdWarning");
    mtm1m3ts.salEventPub((char*)"MTM1M3TS_logevent_windSensorWarning");
    mtm1m3ts.salTelemetryPub((char*)"MTM1M3TS_flowData");
    mtm1m3ts.salTelemetryPub((char*)"MTM1M3TS_mixingValveData");
    mtm1m3ts.salTelemetryPub((char*)"MTM1M3TS_powerSupplyData");
    mtm1m3ts.salTelemetryPub((char*)"MTM1M3TS_thermalActuatorData");
    mtm1m3ts.salTelemetryPub((char*)"MTM1M3TS_thermalScanner1Data");
    mtm1m3ts.salTelemetryPub((char*)"MTM1M3TS_thermalScanner2Data");
    mtm1m3ts.salTelemetryPub((char*)"MTM1M3TS_thermalScanner3Data");
    mtm1m3ts.salTelemetryPub((char*)"MTM1M3TS_thermalScanner4Data");
    mtm1m3ts.salTelemetryPub((char*)"MTM1M3TS_vfdData");
    mtm1m3ts.salTelemetryPub((char*)"MTM1M3TS_windData");
}

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */

#endif /* UTILITY_MTM1M3TS_H_ */