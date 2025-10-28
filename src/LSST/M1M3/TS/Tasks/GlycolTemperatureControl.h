/*
 * Task controlling mixing valve position.
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

#ifndef _TS_Tasks_GlycolTemperatureControl_
#define _TS_Tasks_GlycolTemperatureControl_

#include <chrono>

#include "cRIO/Task.h"
#include "PID/LimitedPID.h"

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Tasks {

/***
 * Task class controlling glycol loop temperature. Set mixing valve opening ratio.
 *
 * The class uses a state machine to properly compensate for a backlash. It was
 * observed the mixing valve cannot be commanded by a small increments. But
 * commanding a bigger opening, and then returning to the target value, allows
 * finer control.
 */
class GlycolTemperatureControl : public cRIO::Task {
public:
    GlycolTemperatureControl();

    virtual cRIO::task_return_t run();

    float target_mixing_valve = 0;

    PID::LimitedPID target_pid;
};

}  // namespace Tasks
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // ! _TS_Tasks_GlycolTemperatureControl_
