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

#ifndef DOMAIN_DOMAIN_H_
#define DOMAIN_DOMAIN_H_

#include "Defines.h"
#include "Functions.h"
#include "Limits.h"

namespace LSST {
namespace TS {
namespace MTM1M3TS {

enum class InterlockSystemStates {
	NoStateTransition,
	NoStateTransitionInvalidState,
	NoStateTransitionInvalidParameter,
	Standby,
	Disabled,
	Fault,
};

struct InterlockSystemSettings {
	AllBitNotSetFunction RawSampleToFanCoilHeatersOffInterlock = AllBitNotSetFunction(0x00010000);
	AllBitNotSetFunction RawSampleToCoolantPumpOffInterlock = AllBitNotSetFunction(0x00020000);
	AllBitNotSetFunction RawSampleToGISHeartbeatLostInterlock = AllBitNotSetFunction(0x00040000);
	AllBitNotSetFunction RawSampleToMixingValveClosedInterlock = AllBitNotSetFunction(0x00080000);
	AllBitNotSetFunction RawSampleToSupportSystemHeartbeatLostInterlock = AllBitNotSetFunction(0x00100000);
	AllBitNotSetFunction RawSampleToCellDoorOpenInterlock = AllBitNotSetFunction(0x00200000);
	AllBitNotSetFunction RawSampleToGISEarthquakeInterlock = AllBitNotSetFunction(0x00400000);
	AllBitNotSetFunction RawSampleToCoolantPumpEStopInterlock = AllBitNotSetFunction(0x00800000);
	AllBitNotSetFunction RawSampleToCabinetOverTempInterlock = AllBitNotSetFunction(0x01000000);

	EqualLimit FanCoilHeatersOffInterlockLimit = EqualLimit(1, Limits::WARNING);
	EqualLimit CoolantPumpOffInterlockLimit = EqualLimit(1, Limits::WARNING);
	EqualLimit GISHeartbeatLostInterlockLimit = EqualLimit(1, Limits::WARNING);
	EqualLimit MixingValveClosedInterlockLimit = EqualLimit(1, Limits::WARNING);
	EqualLimit SupportSystemHeartbeatLostInterlockLimit = EqualLimit(1, Limits::WARNING);
	EqualLimit CellDoorOpenInterlockLimit = EqualLimit(1, Limits::WARNING);
	EqualLimit GISEarthquakeInterlockLimit = EqualLimit(1, Limits::FAULT);
	EqualLimit CoolantPumpEStopInterlockLimit = EqualLimit(1, Limits::FAULT);
	EqualLimit CabinetOverTempInterlockLimit = EqualLimit(1, Limits::WARNING);

	uint32_t HeartbeatTogglePeriod = 10;
};

struct InterlockSystemData {
	InterlockSystemStates State;

	double SampleTimestamp;
	uint32_t RawSample;
	bool FanCoilHeatersOffInterlock;
	bool CoolantPumpOffInterlock;
	bool GISHeartbeatLostInterlock;
	bool MixingValveClosedInterlock;
	bool SupportSystemHeartbeatLostInterlock;
	bool CellDoorOpenInterlock;
	bool GISEarthquakeInterlock;
	bool CoolantPumpEStopInterlock;
	bool CabinetOverTempInterlock;

	Limits FanCoilHeatersOffInterlockLimit;
	Limits CoolantPumpOffInterlockLimit;
	Limits GISHeartbeatLostInterlockLimit;
	Limits MixingValveClosedInterlockLimit;
	Limits SupportSystemHeartbeatLostInterlockLimit;
	Limits CellDoorOpenInterlockLimit;
	Limits GISEarthquakeInterlockLimit;
	Limits CoolantPumpEStopInterlockLimit;
	Limits CabinetOverTempInterlockLimit;

	bool HeartbeatCommand;
};

enum class MainSystemStates {
	NoStateTransition = 0,
	NoStateTransitionInvalidState = 1,
	NoStateTransitionInvalidParameter = 2,
	Offline = 3,
	Standby = 4,
	Disabled = 5,
	Enabled = 6,
	Engineering = 7,
	Fault = 8
};

struct MainSystemSettings { 

};

struct MainSystemData {
	MainSystemStates State;

};

enum class SubsystemResults {
	Ok = 0,
	InvalidState = 1,
	InvalidParameter = 2,
	Fault = 3
};

static inline SubsystemResults merge(SubsystemResults a, SubsystemResults b) { return a >= b ? a : b; }

struct ThermalFPGASystemSettings {

};

struct ThermalFPGASystemData {
	uint64_t Reserved;
	uint64_t AnalogInputSampleCount;
	uint64_t AnalogInputSampleTimestamp;
	float AnalogInputRaw1;
	float AnalogInputRaw2;
	float AnalogInputRaw3;
	float AnalogInputRaw4;
	float AnalogInputRaw5;
	float AnalogInputRaw6;
	float AnalogInputRaw7;
	float AnalogInputRaw8;
	float AnalogInputRaw9;
	float AnalogInputRaw10;
	float AnalogInputRaw11;
	float AnalogInputRaw12;
	float AnalogInputRaw13;
	float AnalogInputRaw14;
	float AnalogInputRaw15;
	float AnalogInputRaw16;
	uint64_t DigitalInputSampleCount;
	uint64_t DigitalInputSampleTimestamp;
	uint32_t DigitalInputStates;
	uint64_t ThermocoupleSampleCount;
	uint64_t ThermocoupleSampleTimestamp;
	float ThermocoupleRaw1;
	float ThermocoupleRaw2;
	float ThermocoupleRaw3;
	float ThermocoupleRaw4;
	float ThermocoupleRaw5;
	float ThermocoupleRaw6;
	float ThermocoupleRaw7;
	float ThermocoupleRaw8;
	float ThermocoupleRaw9;
	float ThermocoupleRaw10;
	float ThermocoupleRaw11;
	float ThermocoupleRaw12;
	float ThermocoupleRaw13;
	float ThermocoupleRaw14;
	float ThermocoupleRaw15;
	float ThermocoupleRaw16;
	uint64_t WindSensorTxBytes;
	uint64_t WindSensorTxFrames;
	uint64_t WindSensorRxBytes;
	uint64_t WindSensorRxFrames;
	uint64_t WindSensorErrorTimestamp;
	uint8_t WindSensorErrorCode;
	uint64_t WindSensorSampleTimestamp;
	int32_t WindSensorXRaw;
	int32_t WindSensorYRaw;
	int32_t WindSensorZRaw;
	int32_t WindSensorSonicTempRaw;
	uint16_t WindSensorDiagWord;
	uint8_t WindSensorRecordCount;
	uint64_t FlowSensorTxBytes;
	uint64_t FlowSensorTxFrames;
	uint64_t FlowSensorRxBytes;
	uint64_t FlowSensorRxFrames;
	uint64_t FlowSensorErrorTimestamp;
	uint8_t FlowSensorErrorCode;
	uint64_t FlowSensorSampleTimestamp;
	float FlowSensorSignalStrength;
	float FlowSensorFlowRate;
	float FlowSensorNetTotalizer;
	float FlowSensorPositiveTotalizer;
	float FlowSensorNegativeTotalizer;
	float FlowSensorTemperature1;
	float FlowSensorTemperature2;
	uint64_t VFDTxBytes;
	uint64_t VFDTxFrames;
	uint64_t VFDRxBytes;
	uint64_t VFDRxFrames;
	uint64_t VFDErrorTimestamp;
	uint8_t VFDErrorCode;
	uint64_t VFDSampleTimestamp;
	uint16_t VFDVelocityBits;
	uint16_t VFDDiagnosticCode;
	uint16_t VFDFrequencyCommand;
	uint16_t OutputFrequency;
	uint16_t OutputCurrent;
	uint16_t DCBusVoltage;
	uint16_t OutputVoltage;
};

} /* namespace MTM1M3TS */
} /* namespace TS */
} /* namespace LSST */

#endif /* DOMAIN_DOMAIN_H_ */
