/*
 * Tasks controlling mixing valve position.
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

#include "cRIO/Task.h"
#include "PID/LimitedPID.h"

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Tasks {

class GlycolTemperatureControl : public cRIO::Task {
public:
    GlycolTemperatureControl();

    virtual cRIO::task_return_t run();

    float target_mixing_valve = 0;

    PID::LimitedPID target_pid;

private:
    int safety_violations;
};

}  // namespace Tasks
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // ! _TS_Tasks_GlycolTemperatureControl_
