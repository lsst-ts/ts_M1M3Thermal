/*
 * Flow Meter telemetry handling class.
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

#include <cmath>

#include <spdlog/spdlog.h>

#include <IFPGA.h>
#include <TSPublisher.h>
#include <Telemetry/FlowMeter.h>

using namespace LSST::M1M3::TS::Telemetry;
using namespace std::chrono_literals;

FlowMeter::FlowMeter(token) {
    signalStrength = NAN;
    flowRate = NAN;
    netTotalizer = NAN;
    positiveTotalizer = NAN;
    negativeTotalizer = NAN;
}

void FlowMeter::update() {
    auto flowMeter = IFPGA::get().flowMeter;

    flowMeter->clearCommanded();

    flowMeter->poll();

    IFPGA::get().mpuCommands(*flowMeter, 2s);

    signalStrength = flowMeter->getSignalStrength();
    flowRate = flowMeter->getFlowRate();
    netTotalizer = flowMeter->getNetTotalizer();
    positiveTotalizer = flowMeter->getPositiveTotalizer();
    negativeTotalizer = flowMeter->getNegativeTotalizer();
}

void FlowMeter::send() {
    salReturn ret = TSPublisher::SAL()->putSample_flowMeter(this);
    if (ret != SAL__OK) {
        SPDLOG_WARN("Cannot send FlowMeter: {}", ret);
        return;
    }
}
