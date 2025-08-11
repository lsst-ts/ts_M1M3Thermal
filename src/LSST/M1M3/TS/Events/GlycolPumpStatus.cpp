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

#include <algorithm>

#include <spdlog/spdlog.h>

#include "Events/ErrorCode.h"
#include "Events/GlycolPumpStatus.h"
#include "Events/SummaryState.h"
#include "IFPGA.h"
#include "TSPublisher.h"

using namespace LSST::M1M3::TS::Events;

GlycolPumpStatus::GlycolPumpStatus(token) {
    _last_status = 0xFFFF;
    _last_errorCode = 0xFFFF;
    _error_count = 0;
}

void GlycolPumpStatus::update(VFD *vfd) {
    // mask unused bits
    auto status = vfd->getStatus() & 0x0ebf;
    errorCode = vfd->getDriveErrorCodes();

    // pump failed
    if (errorCode != 0) {
        static int auto_reset[] = {2, 4, 5, 6, 7, 8, 9, 13, 21, 29, 48, 59, 63};
        static int non_resetable[] = {3,   12,  15,  33,  38,  39,  40,  41,  42,  43,  64,  70,
                                      71,  72,  73,  80,  81,  82,  83,  91,  94,  100, 101, 105,
                                      106, 107, 109, 110, 111, 114, 122, 125, 126, 127};

        if (std::find(std::begin(non_resetable), std::end(non_resetable), errorCode) !=
            std::end(non_resetable)) {
            Events::SummaryState::instance().fail(
                    Events::ErrorCode::EGWPump, "Non-resettable EGW pump error " + std::to_string(errorCode),
                    "");
        } else if (std::find(std::begin(auto_reset), std::end(auto_reset), errorCode) !=
                   std::end(auto_reset)) {
            if (errorCode != _last_errorCode) {
                SPDLOG_WARN("Auto resetting pump error {}", errorCode);
                TSPublisher::instance().pump_thread->reset_pump();
            } else {
                _error_count++;
                if (_error_count > 5) {
                    Events::SummaryState::instance().fail(
                            Events::ErrorCode::EGWPump,
                            "Cannot reset EGW pump error " + std::to_string(errorCode), "");
                }
            }
        } else {
            Events::SummaryState::instance().fail(Events::ErrorCode::EGWPump,
                                                  "Unknown EGW pump error " + std::to_string(errorCode), "");
        }
    } else {
        _error_count = 0;
    }

    if (status != _last_status || errorCode != _last_errorCode) {
        // bits are from VFD manual
        ready = status & 0x0001;
        running = status & 0x0002;
        forwardCommanded = status & 0x0004;
        forwardRotating = status & 0x0008;
        accelerating = status & 0x0010;
        decelerating = status & 0x0020;
        // 0x0040 is unused
        faulted = status & 0x0080;
        // 0x0100 at reference
        mainFrequencyControlled = status & 0x0200;
        operationCommandControlled = status & 0x0400;
        parametersLocked = status & 0x0800;

        salReturn ret = TSPublisher::SAL()->putSample_logevent_glycolPumpStatus(&instance());
        if (ret != SAL__OK) {
            SPDLOG_WARN("Can not send GlycolPumpStatus: {}", ret);
            return;
        }

        _last_status = status;
        _last_errorCode = errorCode;
    }
}
