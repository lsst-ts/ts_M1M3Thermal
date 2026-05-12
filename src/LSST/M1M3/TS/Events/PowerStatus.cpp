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

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

#include "Events/PowerStatus.h"
#include "TSPublisher.h"

using namespace LSST::M1M3::TS::Events;

PowerStatus::PowerStatus(token) { _updated = true; }

void PowerStatus::set_heaters_power(bool on) {
    if (fanCoilsHeatersCommandedOn != on) {
        fanCoilsHeatersCommandedOn = on;
        _updated = true;
    }
}

void PowerStatus::set_pump_power(bool on) {
    if (coolantPumpCommandedOn != on) {
        coolantPumpCommandedOn = on;
        _updated = true;
    }
}

void PowerStatus::send() {
    if (_updated) {
        TSPublisher::instance().logPowerStatus(this);
    }
}
