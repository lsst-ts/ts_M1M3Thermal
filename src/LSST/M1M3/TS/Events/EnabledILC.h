/*
 * EnabledILC event handling class.
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

#ifndef _TS_Event_EnabledILCILC_
#define _TS_Event_EnabledILCILC_

#include <SAL_MTM1M3TS.h>
#include <cRIO/Singleton.h>

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Events {

class EnabledILC final : MTM1M3TS_logevent_enabledILCC, public cRIO::Singleton<EnabledILC> {
public:
    EnabledILC(token);

    /**
     * Clears failed ILC counts.
     */
    void reset();

    /**
     * Enabled / disable ILC.
     */
    void setEnabled(uint8_t ilc, bool newState);

    bool isEnabled(uint8_t ilc);

    /**
     * Called when an ILC experienced communication problem. Auto disable ILC
     * if communication errors cross a counter.
     */
    void communicationProblem(uint8_t ilc);

    /**
     * Sends updates through SAL/DDS.
     */
    void send();

    // TODO move to SAL/DDS
    int errorCount[cRIO::NUM_TS_ILC];
    bool autoDisabled[cRIO::NUM_TS_ILC];

private:
    bool _updated;
};

}  // namespace Events
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // !_TS_Event_EnabledILCILC_
