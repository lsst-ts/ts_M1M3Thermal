/*
 * MixingValve telemetry handling class.
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

#include <Settings/MixingValve.h>
#include <TSPublisher.h>
#include <Telemetry/MixingValve.h>
#include <cRIO/ThermalILC.h>
#include <spdlog/spdlog.h>

#include <cmath>

using namespace LSST::M1M3::TS;
using namespace LSST::M1M3::TS::Telemetry;

MixingValve::MixingValve(token) { rawValvePosition = NAN; }

void MixingValve::sendPosition(float position) {
    rawValvePosition = position;
    valvePosition = Settings::MixingValve::instance().positionToPercents(position);

    salReturn ret = TSPublisher::SAL()->putSample_mixingValve(this);
    if (ret != SAL__OK) {
        SPDLOG_WARN("Cannot send mixingValve: {}", ret);
        return;
    }
}
