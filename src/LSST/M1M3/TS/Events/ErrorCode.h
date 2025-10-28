/*
 * TS ErrorCode.
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

#ifndef _TS_Event_ErrorCodes_
#define _TS_Event_ErrorCodes_

#include <string>

#include <SAL_MTM1M3TS.h>

#include "cRIO/Singleton.h"
#include "TSPublisher.h"

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Events {

class ErrorCode : MTM1M3TS_logevent_errorCodeC, public cRIO::Singleton<ErrorCode> {
public:
    enum Type {
        NoFault = 0,
        EGWPump = 1,
        TemperatureSensors = 2,
        EGWPumpStartup = 3,
        MixingValveTimeout = 4,
        MixingValveMovedOutOfTarget = 5
    };

    ErrorCode(token);

    void set(int error_code, const std::string &error_report, const std::string &traceback);

    void clear(const std::string &error_report);

    void send() { TSPublisher::instance().log_error_code(this); }
};

}  // namespace Events
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // ! _TS_Event_ErrorCodes_
