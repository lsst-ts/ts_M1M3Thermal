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

class FinerControl : public cRIO::Singleton<FinerControl> {
public:
    FinerControl(token);

    void set_target(float demand);

    float get_target(float valve_position);

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
