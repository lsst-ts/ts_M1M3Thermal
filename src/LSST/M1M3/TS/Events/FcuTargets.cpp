/*
 * Publish AppliedSetpoints event.
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

#include <cRIO/ThermalILC.h>

#include <Events/FcuTargets.h>
#include <TSPublisher.h>

using namespace LSST::M1M3::TS::Events;

FcuTargets::FcuTargets(token) {
    for (int i = 0; i < cRIO::NUM_TS_ILC; i++) {
        heaterPWM[i] = 0;
        fanRPM[i] = 0;
    }
    _updated = true;
}

void FcuTargets::send() {
    if (_updated == false) {
        return;
    }
    salReturn ret = TSPublisher::SAL()->putSample_logevent_fcuTargets(this);
    if (ret != SAL__OK) {
        SPDLOG_WARN("Cannot publish fcuTargets: {}", ret);
        return;
    }
    _updated = false;
}

void FcuTargets::set_fcu_targets(std::vector<float> new_heater_pwm, std::vector<int> new_fan_rpm) {
    for (int i = 0; i < cRIO::NUM_TS_ILC; i++) {
        if (heaterPWM[i] != new_heater_pwm[i]) {
            heaterPWM[i] = new_heater_pwm[i];
            _updated = true;
        }
        if (fanRPM[i] != new_fan_rpm[i]) {
            fanRPM[i] = new_fan_rpm[i];
            _updated = true;
        }
    }
    send();
}
