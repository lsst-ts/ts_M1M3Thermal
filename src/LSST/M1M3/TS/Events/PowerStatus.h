/*
 * Publish MPU Glycol Pump status.
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

#ifndef _TS_Event_PowerStatus_
#define _TS_Event_PowerStatus_

#include <SAL_MTM1M3TS.h>
#include <cRIO/Singleton.h>

#include <MPU/VFD.h>

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Events {

/**
 * Provides data about power state of various M1M3 TS power buses. This only
 * sends telemetry - actual (physical) power is commanded through IFPGA class.
 */
class PowerStatus final : MTM1M3TS_logevent_powerStatusC, public cRIO::Singleton<PowerStatus> {
public:
    PowerStatus(token);

    /**
     * Changes heaters/FCU power status. This is a breaker that controls power delivery
     * to (independent) power bus, powering heaters on FCU. Each FCU comes with
     * total 100 W (there are two heaters, 40 W and 60 W each), so tolal is 9.6
     * kW of power.
     *
     * @param on if true, power is fed to heaters.
     */
    void set_heaters_power(bool on);

    /**
     * Changes EGW VFD power on status.
     *
     * @param on if true, power is fed to EGW VFD.
     */
    void set_pump_power(bool on);

    /**
     * Returns pump power state.
     *
     * @return true if pump VFD is powered on.
     */
    bool pump_on() { return coolantPumpCommandedOn; }

    /**
     * Sends telemetry updates to SAL.
     */
    void send();

private:
    bool _updated;
};

}  // namespace Events
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  //!_TS_Event_PowerStatus_
