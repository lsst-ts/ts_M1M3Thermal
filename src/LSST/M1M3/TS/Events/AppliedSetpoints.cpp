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

#include <Events/AppliedSetpoints.h>
#include <TSPublisher.h>

using namespace LSST::M1M3::TS::Events;

AppliedSetpoints::AppliedSetpoints(token) { reset(); }

void AppliedSetpoints::reset() {
    glycolSetpoint = NAN;
    heatersSetpoint = NAN;
    _updated = false;
}

void AppliedSetpoints::send() {
    if (_updated == false) {
        return;
    }
    salReturn ret = TSPublisher::SAL()->putSample_logevent_appliedSetpoints(this);
    if (ret != SAL__OK) {
        SPDLOG_WARN("Cannot publish appliedSetpoint: {}", ret);
        return;
    }
    _updated = false;
}

void AppliedSetpoints::setAppliedSetpoints(float new_glycol_setpoint, float new_heaters_setpoint) {
    if (glycolSetpoint != new_glycol_setpoint) {
        glycolSetpoint = new_glycol_setpoint;
        _updated = true;
    }
    if (heatersSetpoint != new_heaters_setpoint) {
        heatersSetpoint = new_heaters_setpoint;
        _updated = true;
    }
}
