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

#ifndef _TS_Events_ThermalInfo_
#define _TS_Events_ThermalInfo_

#include <SAL_MTM1M3TS.h>

#include <cRIO/Singleton.h>

#include <TSPublisher.h>

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Events {

class ThermalInfo : public MTM1M3TS_logevent_thermalInfoC, public cRIO::Singleton<ThermalInfo> {
public:
    ThermalInfo(token);

    void reset();
    void processServerID(uint8_t address, uint8_t ilcIndex, uint64_t uniqueID, uint8_t ilcAppType,
                         uint8_t networkNodeType, uint8_t ilcSelectedOptions, uint8_t networkNodeOptions,
                         uint8_t majorRev, uint8_t minorRev, std::string firmwareName);

    void log() { TSPublisher::instance().logThermalInfo(this); }
};

}  // namespace Events
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // !_TS_Events_ThermalInfo_
