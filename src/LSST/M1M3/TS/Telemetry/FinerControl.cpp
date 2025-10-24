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

#include "Events/ErrorCode.h"
#include "Events/SummaryState.h"
#include "Settings/MixingValve.h"
#include "Telemetry/FinerControl.h"
#include "Telemetry/MixingValve.h"

using namespace LSST::M1M3::TS::Telemetry;

FinerControl::FinerControl(token) {
    _move_timeout =
            std::chrono::steady_clock::now() +
            2 * std::chrono::milliseconds((int)(Settings::MixingValve::instance().maxMovingTime * 1000));
    _comp_setpoint = NAN;
    _last_setpoint = 0;
    state = ON_TARGET;
}

void FinerControl::set_target(float demand) {
    std::lock_guard<std::mutex> lock_g(_lock);

    auto backlash_step = Settings::MixingValve::instance().backlashStep;

    if (demand != _last_setpoint) {
        if (abs(demand - _last_setpoint) > Settings::MixingValve::instance().minimalMove) {
            state = MOVING_TO_TARGET;
        } else {
            state = MOVING_TO_COMPENSATED_TARGET;
            if (demand < _last_setpoint) {
                if (demand < backlash_step) {
                    _comp_setpoint = _last_setpoint + backlash_step;
                } else {
                    _comp_setpoint = demand - backlash_step;
                }
            } else {
                if (demand > (100 - backlash_step)) {
                    _comp_setpoint = _last_setpoint - backlash_step;
                } else {
                    _comp_setpoint = demand + backlash_step;
                }
            }
        }
        _last_setpoint = demand;
        _move_timeout =
                std::chrono::steady_clock::now() +
                std::chrono::milliseconds((int)(Settings::MixingValve::instance().maxMovingTime * 1000));
    }
}

float FinerControl::get_target(float valve_position) {
    std::lock_guard<std::mutex> lock_g(_lock);

    auto now = std::chrono::steady_clock::now();

    auto &mixing_settings = Settings::MixingValve::instance();

    // do not change state for at least two seconds, as the valve moves with delay
    bool transition =
            (_move_timeout - now) < std::chrono::milliseconds((int)mixing_settings.maxMovingTime * 800);

    /**
     * Runs state machine, based on what the valve shall do.
     */
    switch (state) {
        // when in idle and setpint changed, change state.
        case MOVING_TO_COMPENSATED_TARGET:
            if (abs(valve_position - _comp_setpoint) < mixing_settings.inPosition && transition) {
                state = MOVING_TO_TARGET;
                _move_timeout = now + std::chrono::milliseconds((int)(mixing_settings.maxMovingTime * 1000));
                return _last_setpoint;
            }
            if (now >= _move_timeout) {
                state = FAULTED;
                Events::SummaryState::instance().fail(
                        Events::ErrorCode::MixingValveTimeout,
                        fmt::format("Timeout moving to compensated target value "
                                    "(valve_position is {}, demand is {}).",
                                    valve_position, _comp_setpoint),
                        "");
                return NAN;
            }
            return _comp_setpoint;
        case MOVING_TO_TARGET:
            if (abs(valve_position - _last_setpoint) < mixing_settings.inPosition && transition) {
                state = ON_TARGET;
                return NAN;
            } else if (now >= _move_timeout) {
                Events::SummaryState::instance().fail(
                        Events::ErrorCode::MixingValveTimeout,
                        fmt::format("Timeout moving to target value (valve_position is {}, demand is {}).",
                                    valve_position, _last_setpoint),
                        "");
                state = FAULTED;
                return NAN;
            }
            return _last_setpoint;
        case ON_TARGET:
            if (abs(valve_position - _last_setpoint) >= mixing_settings.inPosition) {
                Events::SummaryState::instance().fail(
                        Events::ErrorCode::MixingValveMovedOutOfTarget,
                        fmt::format("Moved out of target while on target: {}, demand was {}.", valve_position,
                                    _last_setpoint),
                        "");
                state = FAULTED;
                return NAN;
            }
            return NAN;
        case FAULTED:
            return NAN;
        default:
            return _last_setpoint;
    }
}
