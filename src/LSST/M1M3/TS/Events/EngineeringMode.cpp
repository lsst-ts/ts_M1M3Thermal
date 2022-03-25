/*
 * EngineeringMode event.
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

#include <TSPublisher.h>
#include <Events/EngineeringMode.h>
#include <spdlog/spdlog.h>

using namespace LSST::M1M3::TS;
using namespace LSST::M1M3::TS::Events;

EngineeringMode::EngineeringMode(token) {
    engineeringMode = false;
    send();
}

void EngineeringMode::setEnabled(bool newState) {
    if (engineeringMode != newState) {
        engineeringMode = newState;
        send();
    }
}

bool EngineeringMode::isEnabled() { return engineeringMode; }

void EngineeringMode::send() {
    salReturn ret = TSPublisher::SAL()->putSample_logevent_engineeringMode(this);
    if (ret != SAL__OK) {
        SPDLOG_WARN("Cannot send engineeringMode: {}", ret);
        return;
    }
}
