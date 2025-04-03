/*
 * Application global variables.
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

#include "TSApplication.h"

#include "Events/EnabledILC.h"
#include "Events/FcuTargets.h"

using namespace LSST::M1M3::TS;

void TSApplication::callFunctionOnAllIlcs(std::function<void(uint8_t)> func) {
    for (int address = 1; address <= LSST::cRIO::NUM_TS_ILC; address++) {
        if (Events::EnabledILC::instance().isEnabled(address - 1)) {
            func(address);
        }
    }
}

void TSApplication::set_FCU_heaters_fans(const std::vector<int> &heater_PWM,
                                         const std::vector<int> &fan_RPM) {
    ilc()->clear();

    callFunctionOnAllIlcs([heater_PWM, fan_RPM](uint8_t address) -> void {
        TSApplication::ilc()->setThermalDemand(address, heater_PWM[address - 1], fan_RPM[address - 1]);
    });

    IFPGA::get().ilcCommands(*TSApplication::ilc(), 1000);

    std::vector<float> target_heater_PWM(cRIO::NUM_TS_ILC);
    std::vector<int> target_fan_RPM(cRIO::NUM_TS_ILC);

    for (int i = 0; i < cRIO::NUM_TS_ILC; i++) {
        target_heater_PWM[i] = 100.0 * (heater_PWM[i] / 255.0);
        target_fan_RPM[i] = fan_RPM[i] * 10;
    }
    Events::FcuTargets::instance().set_fcu_targets(target_heater_PWM, target_fan_RPM);

    SPDLOG_DEBUG("TSApplication::set_FCU_heaters_fans Changed heaters and fans demand: {:.1f} % {:d}",
                 target_heater_PWM[0], target_fan_RPM[0]);
}
