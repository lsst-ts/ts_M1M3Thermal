/*
 * VFD EGW Pump Driver Status2
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

#ifndef _TS_Event_DriveStatus2_
#define _TS_Event_DriveStatus2_

#include <string>

#include <SAL_MTM1M3TS.h>

#include "cRIO/Singleton.h"
#include "TSPublisher.h"

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Events {

class DriveStatus2 : MTM1M3TS_logevent_driveStatus2C, public cRIO::Singleton<DriveStatus2> {
public:
    enum STATUS2 {
        JOGGING = 0x0001,
        FLUX_BREAKING = 0x0002,
        MOTOR_OVERLOAD = 0x0004,
        AUTORST_CTDN = 0x0008,
        DC_BRAKING = 0x0010,
        AT_FREQUENCY = 0x0020,
        AUTO_TUNING = 0x0040,
        EM_BRAKING = 0x0080,
        CURRENT_LIMIT = 0x0100,
        // NA - 0x0200
        SAFETY_S1 = 0x0400,
        SAFETY_S2 = 0x0800,
        F111_STATUS = 0x1000,
        SAFETQPERMIT = 0x2000
        // NA - 0x4000
        // NA - 0x8000
    };

    DriveStatus2(token);

    void set(uint16_t _drive_status);
    void send() { TSPublisher::instance().log_drive_status_2(this); }

private:
    uint16_t drive_status_2;
};

}  // namespace Events
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // ! _TS_Event_DriveStatus2_
