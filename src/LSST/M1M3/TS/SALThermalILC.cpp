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

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <SALThermalILC.h>

#include <Events/ThermalInfo.h>
#include <Events/EnabledILC.h>
#include <Settings/Thermal.h>
#include <Telemetry/ThermalData.h>

using namespace LSST::M1M3::TS;

SALThermalILC::SALThermalILC(std::shared_ptr<SAL_MTM1M3TS> m1m3tsSAL)
        : ILC::ILCBusList(1), cRIO::ThermalILC(1), _m1m3tsSAL(m1m3tsSAL) {}

#if 0
void SALThermalILC::handleMissingReply(uint8_t address, uint8_t func) {
    if (Settings::Thermal::instance().autoDisable) {
        Events::EnabledILC::instance().communicationProblem(_address2ILCIndex(address));
    } else {
        cRIO::ThermalILC::handleMissingReply(address, func);
    }
}
#endif

void SALThermalILC::processServerID(uint8_t address, uint64_t uniqueID, uint8_t ilcAppType,
                                    uint8_t networkNodeType, uint8_t ilcSelectedOptions,
                                    uint8_t networkNodeOptions, uint8_t majorRev, uint8_t minorRev,
                                    std::string firmwareName) {
    uint8_t ilcIndex = _address2ILCIndex(address);
    Events::ThermalInfo::instance().processServerID(address, ilcIndex, uniqueID, ilcAppType, networkNodeType,
                                                    ilcSelectedOptions, networkNodeOptions, majorRev,
                                                    minorRev, firmwareName);
}

void SALThermalILC::processServerStatus(uint8_t address, uint8_t mode, uint16_t status, uint16_t faults) {}

void SALThermalILC::processChangeILCMode(uint8_t address, uint16_t mode) {}

void SALThermalILC::processSetTempILCAddress(uint8_t address, uint8_t newAddress) {}

void SALThermalILC::processResetServer(uint8_t address) { SPDLOG_DEBUG("ILC {} server reset.", address); }

void SALThermalILC::processThermalStatus(uint8_t address, uint8_t status, float differentialTemperature,
                                         uint8_t fanRPM, float absoluteTemperature) {
    Telemetry::ThermalData::instance().update(address, status, differentialTemperature, fanRPM,
                                              absoluteTemperature);
}

void SALThermalILC::processReHeaterGains(uint8_t address, float proportionalGain, float integralGain) {
    throw std::runtime_error(
            fmt::format("Re-Heater gain should not be called - called for address {}", +address));
}

uint8_t SALThermalILC::_address2ILCIndex(uint8_t address) { return address - 1; }
