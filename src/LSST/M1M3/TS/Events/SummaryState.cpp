/*
 * SummaryState event handling class.
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

#include <spdlog/fmt/fmt.h>

#include "Events/ErrorCode.h"
#include "Events/SummaryState.h"
#include "IFPGA.h"
#include "Telemetry/FinerControl.h"

using namespace LSST::M1M3::TS::Events;
using namespace MTM1M3TS;

SummaryState::SummaryState(token) : _updated(false) {
    summaryState = MTM1M3TS_shared_SummaryStates_OfflineState;
}

void SummaryState::send() {
    if (instance()._updated == false) {
        return;
    }
    salReturn ret = TSPublisher::SAL()->putSample_logevent_summaryState(&instance());
    if (ret != SAL__OK) {
        SPDLOG_WARN("Can not send summaryState: {}", ret);
        return;
    }
    instance()._updated = false;
}

bool SummaryState::active() {
    std::lock_guard<std::mutex> lockG(_state_mutex);
    return summaryState == MTM1M3TS_shared_SummaryStates_DisabledState ||
           summaryState == MTM1M3TS_shared_SummaryStates_EnabledState;
}

bool SummaryState::enabled() {
    std::lock_guard<std::mutex> lockG(_state_mutex);
    return summaryState == MTM1M3TS_shared_SummaryStates_EnabledState;
}

void SummaryState::fail(int error_code, const std::string &error_report, const std::string &traceback) {
    _switch_state(MTM1M3TS_shared_SummaryStates_FaultState);
    Events::ErrorCode::instance().set(error_code, error_report, traceback);
    SPDLOG_ERROR("Faulted ({}): {}", error_code, error_report);

    // cleanup - close mixing valve
    try {
        IFPGA::get().panic();
    } catch (std::runtime_error &er) {
        SPDLOG_ERROR("Cannot panic CSC: {}", er.what());
    }
}

void SummaryState::_switch_state(int new_state) {
    std::lock_guard<std::mutex> lockG(_state_mutex);
    SPDLOG_TRACE("SummaryState::_switch_state from {} to {}", summaryState, new_state);
    if (summaryState == new_state) {
        // it is legal to go to fault from fault state, as the code might hit
        if (new_state == MTM1M3TS_shared_SummaryStates_FaultState) {
            return;
        }
        throw std::runtime_error(fmt::format("Already in {} state", new_state));
    }
    switch (new_state) {
        case MTM1M3TS_shared_SummaryStates_DisabledState:
            if (summaryState != MTM1M3TS_shared_SummaryStates_StandbyState &&
                summaryState != MTM1M3TS_shared_SummaryStates_EnabledState) {
                throw std::runtime_error(
                        fmt::format("Cannot switch to Disabled state from {}", summaryState));
            }
            break;
        case MTM1M3TS_shared_SummaryStates_EnabledState:
            if (summaryState != MTM1M3TS_shared_SummaryStates_DisabledState) {
                throw std::runtime_error(fmt::format("Cannot switch to Enable state from {}", summaryState));
            }
            break;
        case MTM1M3TS_shared_SummaryStates_FaultState:
            break;
        case MTM1M3TS_shared_SummaryStates_OfflineState:
            if (summaryState != MTM1M3TS_shared_SummaryStates_StandbyState) {
                SPDLOG_ERROR("Ignored request to switch to offline state from non-Standby state - {}.",
                             summaryState);
                return;
            }
            break;
        case MTM1M3TS_shared_SummaryStates_StandbyState:
            if (summaryState != MTM1M3TS_shared_SummaryStates_DisabledState &&
                summaryState != MTM1M3TS_shared_SummaryStates_OfflineState &&
                summaryState != MTM1M3TS_shared_SummaryStates_FaultState) {
                throw std::runtime_error(fmt::format("Cannot switch to Standby state from {}", summaryState));
            }
            break;
        default:
            throw std::runtime_error(fmt::format("Invalid target state: {}", new_state));
    }
    instance()._updated = true;
    summaryState = new_state;
    send();
}
