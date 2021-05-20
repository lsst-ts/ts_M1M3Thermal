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

#include <TSConstants.h>
#include <TSPublisher.h>
#include <Events/EnabledILC.h>
#include <spdlog/spdlog.h>

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Events {

EnabledILC::EnabledILC(token) : _updated(true) {
    for (int i = 0; i < TSConstants::THERMAL_ILC_COUNT; i++) {
        enabled[i] = true;
    }
}

void EnabledILC::setEnabled(uint8_t ilc, bool newState) {
    if (newState != enabled[ilc]) {
        _updated = true;
        enabled[ilc] = newState;
    }
}

bool EnabledILC::isEnabled(uint8_t ilc) { return enabled[ilc]; }

void EnabledILC::send() {
    if (_updated == false) {
        return;
    }
    salReturn ret = TSPublisher::instance().CSC()->putSample_logevent_enabledILC(this);
    if (ret != SAL__OK) {
        SPDLOG_WARN("Cannot send enabledILC: {}", ret);
        return;
    }
    _updated = false;
}

}  // namespace Events
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST
