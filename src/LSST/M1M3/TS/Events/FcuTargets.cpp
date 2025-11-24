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

#include <algorithm>

#include <spdlog/spdlog.h>

#include <cRIO/ThermalILC.h>

#include "Events/FcuTargets.h"
#include "IFPGA.h"
#include "TSApplication.h"
#include "TSPublisher.h"

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

void FcuTargets::set_FCU_heaters_fans(std::vector<int> _heater_PWM, std::vector<int> _fan_RPM) {
    auto &app = TSApplication::instance();

    app.ilc()->clear();

    app.callFunctionOnAllIlcs([&](uint8_t address) -> void {
        auto i = address - 1;
        int heater = _heater_PWM[i];
        if (heater < 0 || heater > 255) {
            _heater_PWM[i] = heater = heaterPWM[i];
        }
        int fan = _fan_RPM[i];
        if (fan < 0 || fan > 255) {
            _fan_RPM[i] = fan = fanRPM[i];
        }

        TSApplication::ilc()->setThermalDemand(address, heater, fan);
    });

    IFPGA::get().ilcCommands(*TSApplication::ilc(), 1000);

    std::vector<float> target_heater_PWM(cRIO::NUM_TS_ILC);
    std::vector<int> target_fan_RPM(cRIO::NUM_TS_ILC);

    // converts 0-255 values to human readable values
    for (int i = 0; i < cRIO::NUM_TS_ILC; i++) {
        target_heater_PWM[i] = 100.0 * (_heater_PWM[i] / 255.0);
        target_fan_RPM[i] = _fan_RPM[i] * 10;
    }

    _set_fcu_targets(target_heater_PWM, target_fan_RPM);

    const auto [h_min, h_max] = std::minmax_element(begin(target_heater_PWM), end(target_heater_PWM));
    const auto [f_min, f_max] = std::minmax_element(begin(target_fan_RPM), end(target_fan_RPM));

    SPDLOG_DEBUG("Changed targets: heaters {:.1f} % to {:.1f} %, fans {:d} to {:d}.", *h_min, *h_max, *f_min,
                 *f_max);
}

void FcuTargets::recover() {
    std::vector<int> heater(0, cRIO::NUM_TS_ILC);
    std::vector<int> fan(0, cRIO::NUM_TS_ILC);
    for (int i = 0; i < cRIO::NUM_TS_ILC; i++) {
        fan[i] = fanRPM[i] / 10;
    }

    set_FCU_heaters_fans(heater, fan);
}

void FcuTargets::_set_fcu_targets(std::vector<float> new_heater_pwm, std::vector<int> new_fan_rpm) {
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
