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

#ifndef _TS_Events_ThermalWarning_
#define _TS_Events_ThermalWarning_

#include <SAL_MTM1M3TS.h>

#include <cRIO/Singleton.h>

#include "TSPublisher.h"

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Events {

class ThermalWarning : public MTM1M3TS_logevent_thermalWarningC, public cRIO::Singleton<ThermalWarning> {
public:
    ThermalWarning(token);

    void update(uint8_t address, uint8_t mode, uint16_t status, uint16_t faults);

    void send();

private:
    bool _updated;
};

}  // namespace Events
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // !_TS_Events_ThermalWarning_
