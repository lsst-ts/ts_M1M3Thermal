/*
 * SummaryState event handling class.
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

#ifndef _TS_Event_SummaryState_
#define _TS_Event_SummaryState_

#include <SAL_MTM1M3TS.h>
#include <cRIO/Singleton.h>

#include <TSPublisher.h>

#include <spdlog/spdlog.h>
#include <mutex>

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Events {

/**
 * State handling class. Inherits SAL/DDS summary state structure, adds
 * multi-thread safety for state changes.
 */
class SummaryState final : MTM1M3TS_logevent_summaryStateC, public cRIO::Singleton<SummaryState> {
public:
    SummaryState(token);

    /**
     * Returns true if summary state is disabled or enabled (e.g. not standby or fault).
     *
     * @return true if state is DISABLED or ENABLED
     *
     * @multithreading safe
     */
    bool active();

    /**
     * Returns true if summary state is enabled.
     *
     * @return true if state == ENABLED
     *
     * @multithreading safe
     */
    bool enabled();

    /**
     *
     * @multithreading safe
     */
    static void setState(int newState) { instance()._switchState(newState); }
    static void send();

private:
    void _switchState(int newState);
    bool _updated;

    std::mutex _stateMutex;
};

}  // namespace Events
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  //! _TS_Event_SummaryState_
