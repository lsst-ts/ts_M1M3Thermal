/*
 * Finer control for the mixing valve.
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

#ifndef _TS_FinerControl_
#define _TS_FinerControl_

#include <chrono>
#include <mutex>

#include <cRIO/Singleton.h>

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Telemetry {

/***
 * Provides finer control of the mixing valve. Compensates for mixing valve
 * hysteresis.
 *
 * Runs internal state machine, so calls to get_target return proper new
 * targets.
 */
class FinerControl : public cRIO::Singleton<FinerControl> {
public:
    FinerControl(token);

    /***
     * Sets new mxing valve target. Changes state machine either to MOVING_TO_COMPENSATED_TARGET (if
     * compensation target move is required) or to MOVING_TO_TARGET (if far from the new target, and
     * compensation isn't required).
     *
     * @param demand new mixing valve demand, in %.
     */
    void set_target(float demand);

    /***
     * Returns new target, based on the current mixing valve position and internal state.
     *
     * @param valve_position current mixing valve position, in %, read from the input.
     *
     * @return new target for the mixing valve - or NAN if mixing valve shall not be changed.
     */
    float get_target(float valve_position);

    /***
     * State machine states.
     *
     * - MOVING_TO_COMPENSATED_TARGET
     *   the target was to close to the current position, so first move a bit
     *   over (or under) the current position to wake the valve up.
     *
     * - MOVING_TO_TARGET
     *   when moving to new target - either because new demand was far from the
     *   current target, or the mixing valve moved closed enough to compensated
     *   target.
     *
     * - ON_TARGET
     *   when close to demand value. get_target then moves to FAULT if the mixing valve veered too far from
     * the target value.
     *
     * - FAULTED
     *   when something goes wrong.
     */
    enum { MOVING_TO_COMPENSATED_TARGET, MOVING_TO_TARGET, ON_TARGET, FAULTED } state;

private:
    std::mutex _lock;

    std::chrono::time_point<std::chrono::steady_clock> _move_timeout;
    float _comp_setpoint;
    float _last_setpoint;
};

}  // namespace Telemetry
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // !_TS_FinerControl_
