/*
 * SAL commands
 *
 * Developed for the Vera C. Rubin Observatory Telescope & Site Software Systems.
 * This product includes software developed by the Vera C.Rubin Observatory Project
 * (https://www.lsst.org). See the COPYRIGHT file at the top-level directory of
 * this distribution for details of code ownership.
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

#ifndef _TS_Command_SAL
#define _TS_Command_SAL

#include <TSPublisher.h>
#include <SAL_MTM1M3TS.h>

#include <cRIO/SAL/Command.h>

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Commands {

SAL_COMMAND_CLASS_validate(MTM1M3TS, TSPublisher::SAL(), start);

SAL_COMMAND_CLASS(MTM1M3TS, TSPublisher::SAL(), enable);

SAL_COMMAND_CLASS(MTM1M3TS, TSPublisher::SAL(), disable);

SAL_COMMAND_CLASS(MTM1M3TS, TSPublisher::SAL(), standby);

SAL_COMMAND_CLASS(MTM1M3TS, TSPublisher::SAL(), exitControl);

SAL_COMMAND_CLASS_validate(MTM1M3TS, TSPublisher::SAL(), setEngineeringMode);

SAL_COMMAND_CLASS_validate(MTM1M3TS, TSPublisher::SAL(), heaterFanDemand);

SAL_COMMAND_CLASS_validate(MTM1M3TS, TSPublisher::SAL(), setMixingValve);

SAL_COMMAND_CLASS_validate(MTM1M3TS, TSPublisher::SAL(), coolantPumpPower);

SAL_COMMAND_CLASS(MTM1M3TS, TSPublisher::SAL(), coolantPumpStart);

SAL_COMMAND_CLASS(MTM1M3TS, TSPublisher::SAL(), coolantPumpStop);

SAL_COMMAND_CLASS_validate(MTM1M3TS, TSPublisher::SAL(), coolantPumpFrequency);

SAL_COMMAND_CLASS(MTM1M3TS, TSPublisher::SAL(), coolantPumpReset);

}  // namespace Commands
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  //! _TS_Command_SAL
