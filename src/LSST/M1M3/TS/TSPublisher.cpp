/*
 * Thermal System SAL publisher.
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

#include <spdlog/spdlog.h>

using namespace LSST::M1M3::TS;

void TSPublisher::setSAL(std::shared_ptr<SAL_MTM1M3TS> m1m3TSSAL) {
    _m1m3TSSAL = m1m3TSSAL;

    SPDLOG_DEBUG("TSPublisher: Initializing SAL Events");
    _m1m3TSSAL->salTelemetryPub((char*)"MTM1M3TS_logevent_enabledILC");
}
