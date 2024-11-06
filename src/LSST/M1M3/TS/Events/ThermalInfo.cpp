/*
 * EnabledILCs event.
 *
 * Developed for the Vera C. Rubin Observatory Telescope & Site Software
 * Systems. This product includes software developed by the Vera C.Rubin
 * Observatory Project (https://www.lsst.org). See the COPYRIGHT file at the
 * top-level directory of this distribution for details of code ownership.
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

#include <cRIO/ThermalILC.h>

#include <Events/ThermalInfo.h>
#include <Settings/FCUApplicationSettings.h>

using namespace LSST::M1M3::TS::Events;

ThermalInfo::ThermalInfo(token) {
    for (int i = 0; i < cRIO::NUM_TS_ILC; i++) {
        xPosition[i] = Settings::FCUApplicationSettings::Table[i].xPosition;
        yPosition[i] = Settings::FCUApplicationSettings::Table[i].yPosition;
    };
    reset();
}

void ThermalInfo::reset() {
    for (int i = 0; i < cRIO::NUM_TS_ILC; i++) {
        referenceId[i] = 0;
        modbusAddress[i] = 0;
        ilcUniqueId[i] = 0;
        ilcApplicationType[i] = 0;
        networkNodeType[i] = 0;
        majorRevision[i] = 0;
        minorRevision[i] = 0;
    };
}

void ThermalInfo::processServerID(uint8_t _address, uint8_t ilcIndex, uint64_t _uniqueID, uint8_t _ilcAppType,
                                  uint8_t _networkNodeType, uint8_t _ilcSelectedOptions,
                                  uint8_t _networkNodeOptions, uint8_t _majorRev, uint8_t _minorRev,
                                  std::string _firmwareName) {
    referenceId[ilcIndex] = ilcIndex;
    modbusAddress[ilcIndex] = _address;
    ilcUniqueId[ilcIndex] = _uniqueID;
    ilcApplicationType[ilcIndex] = _ilcAppType;
    networkNodeType[ilcIndex] = _networkNodeType;
    majorRevision[ilcIndex] = _majorRev;
    minorRevision[ilcIndex] = _minorRev;
}
