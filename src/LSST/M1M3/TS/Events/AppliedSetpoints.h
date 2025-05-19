/*
 * AppliedSetpoints event handling class.
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

#ifndef _TS_Event_AppliedSetpoints_
#define _TS_Event_AppliedSetpoints_

#include <SAL_MTM1M3TS.h>
#include <cRIO/Singleton.h>

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Events {

class AppliedSetpoints final : MTM1M3TS_logevent_appliedSetpointsC, public cRIO::Singleton<AppliedSetpoints> {
public:
    AppliedSetpoints(token);

    void reset();

    /**
     * Sends updates through SAL/DDS.
     */
    void send();

    void set_applied_setpoints(float new_glycol_setpoint, float new_heaters_setpoint);

    float get_applied_glycol_setpoint() { return glycolSetpoint; }

    float get_applied_heaters_setpoint() { return heatersSetpoint; }

private:
    bool _updated;
};

}  // namespace Events
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // !_TS_Event_AppliedSetpoints_
