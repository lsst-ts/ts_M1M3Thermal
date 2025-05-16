/*
 * GlycolLoopTemperature telemetry handling class.
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

#include <spdlog/spdlog.h>

#include "Events/AppliedSetpoints.h"
#include "Settings/Setpoint.h"
#include "Tasks/Controller.h"
#include "Telemetry/GlycolLoopTemperature.h"
#include "TSPublisher.h"

using namespace LSST::M1M3::TS::Telemetry;

GlycolLoopTemperature::GlycolLoopTemperature(token) {
    aboveMirrorTemperature = NAN;
    insideCellTemperature1 = NAN;
    insideCellTemperature2 = NAN;
    insideCellTemperature3 = NAN;
    telescopeCoolantSupplyTemperature = NAN;
    telescopeCoolantReturnTemperature = NAN;
    mirrorCoolantSupplyTemperature = NAN;
    mirrorCoolantReturnTemperature = NAN;

    _safety_violations_count = 0;
}

void GlycolLoopTemperature::update(const std::vector<float> &temperatures) {
    {
        const std::lock_guard<std::mutex> lock(_access_mutex);

        aboveMirrorTemperature = temperatures[0];
        insideCellTemperature1 = temperatures[1];
        insideCellTemperature2 = temperatures[2];
        insideCellTemperature3 = temperatures[3];
        telescopeCoolantSupplyTemperature = temperatures[4];
        telescopeCoolantReturnTemperature = temperatures[5];
        mirrorCoolantSupplyTemperature = temperatures[6];
        mirrorCoolantReturnTemperature = temperatures[7];
    }

    auto applied_glycol = Events::AppliedSetpoints::instance().get_applied_glycol_setpoint();

    if (!isnan(applied_glycol)) {
        // check M1M3 measurements..
        auto &s_setpoint = Settings::Setpoint::instance();

        float target_temp = get_above_mirror_temperature();
        float t_diff = applied_glycol - target_temp;
        if (abs(t_diff) > s_setpoint.safetyRange) {
            if (_safety_violations_count > s_setpoint.safetyMaxViolations) {
                float new_setpoint;
                if (t_diff > 0) {
                    new_setpoint = target_temp + s_setpoint.safetyRange - s_setpoint.safetyMinOffset;
                } else {
                    new_setpoint = target_temp - s_setpoint.safetyRange + s_setpoint.safetyMinOffset;
                }
                if (s_setpoint.safetyMaxViolations > 0) {
                    float heaters = new_setpoint + s_setpoint.safetyHeatersOffset;
                    SPDLOG_INFO(
                            "Setting glycol setpoint to safe range - was {:.02f}C, difference to air "
                            "{:.02f}, setting to {:.02f}C glycol, {:.02f}C heaters.",
                            applied_glycol, t_diff, new_setpoint, heaters);
                    Tasks::Controller::instance().set_setpoints(new_setpoint, heaters);
                    _safety_violations_count = 0;
                } else {
                    if (_safety_violations_count == s_setpoint.safetyMaxViolations) {
                        SPDLOG_WARN(
                                "Glycol setpoint outside of the safe range - was {:.02f}C, difference to air "
                                "{:.02f}, setting to {:.02f}C.",
                                applied_glycol, t_diff, new_setpoint);
                        _safety_violations_count--;
                    }
                }
            } else {
                SPDLOG_DEBUG("Checked diff: {:.02f} counts {:i}", t_diff, _safety_violations_count);
                _safety_violations_count++;
            }
        } else {
            SPDLOG_TRACE("Glycol temp OK: {:.02f} {:i}", t_diff, _safety_violations_count);
            _safety_violations_count = 0;
        }
    }

    salReturn ret = TSPublisher::SAL()->putSample_glycolLoopTemperature(this);
    if (ret != SAL__OK) {
        SPDLOG_WARN("Cannot send GlycolTemperatureThread: {}", ret);
        return;
    }
}

float GlycolLoopTemperature::get_above_mirror_temperature() {
    std::lock_guard<std::mutex> lock(_access_mutex);
    return aboveMirrorTemperature;
}

float GlycolLoopTemperature::get_mirror_cell_inside_temperature() {
    std::lock_guard<std::mutex> lock(_access_mutex);
    return (insideCellTemperature1 + insideCellTemperature2 + insideCellTemperature3) / 3.0;
}

float GlycolLoopTemperature::get_mirror_loop_average(float supply) {
    std::lock_guard<std::mutex> lock(_access_mutex);
    return (mirrorCoolantSupplyTemperature * supply) + (mirrorCoolantReturnTemperature * (1 - supply));
}

float GlycolLoopTemperature::get_mirror_loop_supply() {
    std::lock_guard<std::mutex> lock(_access_mutex);
    return mirrorCoolantSupplyTemperature;
}

float GlycolLoopTemperature::get_mirror_loop_return() {
    std::lock_guard<std::mutex> lock(_access_mutex);
    return mirrorCoolantReturnTemperature;
}

float GlycolLoopTemperature::get_telescope_loop_supply() {
    std::lock_guard<std::mutex> lock(_access_mutex);
    return telescopeCoolantSupplyTemperature;
}

float GlycolLoopTemperature::get_telescope_loop_return() {
    std::lock_guard<std::mutex> lock(_access_mutex);
    return telescopeCoolantReturnTemperature;
}
