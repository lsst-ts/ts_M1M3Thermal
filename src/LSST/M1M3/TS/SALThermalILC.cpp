/*
 * SAL Thermal ILC.
 *
 * Developed for the Vera C. Rubin Observatory Telescope & Site Software Systems.
 * This product includes software developed by the Vera C.Rubin Observatory Project
 * (https://www.lsst.org). See the COPYRIGHT file at the top-level directory of
 * this distribution for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <SALThermalILC.h>

#include <Telemetry/ThermalData.h>

namespace LSST {
namespace M1M3 {
namespace TS {

SALThermalILC::SALThermalILC(std::shared_ptr<SAL_MTM1M3TS> m1m3tsSAL) : _m1m3tsSAL(m1m3tsSAL) {
    float xyPos[3][2] = {{0, 1}, {1, 2}, {2, 3}};

    for (int i = 0; i < 3; i++) {
        _thermalInfo.xPosition[i] = xyPos[i][0];
        _thermalInfo.yPosition[i] = xyPos[i][1];
        _thermalInfo.zPosition[i] = 0;
    }
}

void SALThermalILC::preProcess() { _thermalInfoChanged = false; }

void SALThermalILC::postProcess() {
    if (_thermalInfoChanged) {
        _m1m3tsSAL->logEvent_thermalInfo(&_thermalInfo, 0);
    }
}

void SALThermalILC::processServerID(uint8_t address, uint64_t uniqueID, uint8_t ilcAppType,
                                    uint8_t networkNodeType, uint8_t ilcSelectedOptions,
                                    uint8_t networkNodeOptions, uint8_t majorRev, uint8_t minorRev,
                                    std::string firmwareName) {
    uint8_t ilcIndex = _address2ILCIndex(address);
    _thermalInfo.referenceId[ilcIndex] = ilcIndex;
    _thermalInfo.modbusAddress[ilcIndex] = address;
    _thermalInfo.ilcUniqueId[ilcIndex] = uniqueID;
    _thermalInfo.ilcApplicationType[ilcIndex] = ilcAppType;
    _thermalInfo.networkNodeType[ilcIndex] = networkNodeType;
    _thermalInfo.majorRevision[ilcIndex] = majorRev;
    _thermalInfo.minorRevision[ilcIndex] = minorRev;

    _thermalInfoChanged = true;
}

void SALThermalILC::processServerStatus(uint8_t address, uint8_t mode, uint16_t status, uint16_t faults) {}

void SALThermalILC::processChangeILCMode(uint8_t address, uint16_t mode) {}

void SALThermalILC::processSetTempILCAddress(uint8_t address, uint8_t newAddress) {}

void SALThermalILC::processResetServer(uint8_t address) {}

void SALThermalILC::processThermalStatus(uint8_t address, uint8_t status, float differentialTemperature,
                                         uint8_t fanRPM, float absoluteTemperature) {
    Telemetry::ThermalData::instance().update(address, status, differentialTemperature, fanRPM,
                                              absoluteTemperature);
}

uint8_t SALThermalILC::_address2ILCIndex(uint8_t address) { return address - 1; }

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST
