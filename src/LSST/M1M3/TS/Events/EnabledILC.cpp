/*
 * EnabledILCs event.
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

#include <spdlog/spdlog.h>

#include <cRIO/ThermalILC.h>

#include <Events/EnabledILC.h>
#include <Settings/Thermal.h>
#include <TSPublisher.h>

using namespace LSST::M1M3::TS;
using namespace LSST::M1M3::TS::Events;

EnabledILC::EnabledILC(token) : _updated(true) { reset(); }

void EnabledILC::reset() {
    for (size_t i = 0; i < cRIO::NUM_TS_ILC; i++) {
        enabled[i] = true;
        autoDisabled[i] = false;
        errorCount[i] = 0;
    }
    _updated = true;
}

void EnabledILC::setEnabled(uint8_t ilc, bool newState) {
    if (newState != enabled[ilc]) {
        _updated = true;
        enabled[ilc] = newState;
    }
}

bool EnabledILC::isEnabled(uint8_t ilc) { return enabled[ilc]; }

void EnabledILC::communicationProblem(uint8_t ilc) {
    errorCount[ilc]++;
    if (Settings::Thermal::instance().autoDisable) {
        if (errorCount[ilc] > Settings::Thermal::instance().failuresToDisable) {
            autoDisabled[ilc] = true;
            setEnabled(ilc, false);
        }
    }
    _updated = true;
}

void EnabledILC::send() {
    if (_updated == false) {
        return;
    }
    salReturn ret = TSPublisher::SAL()->putSample_logevent_enabledILC(this);
    if (ret != SAL__OK) {
        SPDLOG_WARN("Cannot send enabledILC: {}", ret);
        return;
    }
    _updated = false;
}
