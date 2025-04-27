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

#ifndef _TS_Event_FcuTargets_
#define _TS_Event_FcuTargets_

#include <SAL_MTM1M3TS.h>
#include <cRIO/Singleton.h>

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Events {

class FcuTargets final : MTM1M3TS_logevent_fcuTargetsC, public cRIO::Singleton<FcuTargets> {
public:
    FcuTargets(token);

    void reset();

    /**
     * Sends updates through SAL/DDS.
     */
    void send();

    void set_FCU_heaters_fans(const std::vector<int> &heater_PWM, const std::vector<int> &fan_RPM);

    auto get_heaterPWM() { return heaterPWM; }
    auto get_fanRPM() { return fanRPM; }

private:
    bool _updated;

    void _set_fcu_targets(std::vector<float> new_heater_pwm, std::vector<int> new_fan_rpm);
};

}  // namespace Events
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // !_TS_Event_FcuTargets_
