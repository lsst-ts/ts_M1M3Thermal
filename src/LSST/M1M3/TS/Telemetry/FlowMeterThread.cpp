/*
 * Flow Meter telemetry handling class.
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

#include <cmath>

#include <spdlog/spdlog.h>

#include <IFPGA.h>
#include <TSPublisher.h>
#include <Telemetry/FlowMeterThread.h>

using namespace LSST::M1M3::TS::Telemetry;
using namespace std::chrono_literals;

FlowMeterThread::FlowMeterThread(std::shared_ptr<FlowMeter> flowMeter) {
    _flowMeter = flowMeter;
    signalStrength = NAN;
    flowRate = NAN;
    netTotalizer = NAN;
    positiveTotalizer = NAN;
    negativeTotalizer = NAN;
}

void FlowMeterThread::run(std::unique_lock<std::mutex>& lock) {
    SPDLOG_DEBUG("Running Flow Meter Thread.");
    while (keepRunning) {
        auto start = std::chrono::steady_clock::now();

        _flowMeter->readInfo();

        IFPGA::get().mpuCommands(*_flowMeter);

        SPDLOG_TRACE("Sending FlowMeterMPUStatus");

        signalStrength = _flowMeter->getSignalStrength();
        flowRate = _flowMeter->getFlowRate();
        netTotalizer = _flowMeter->getNetTotalizer();
        positiveTotalizer = _flowMeter->getPositiveTotalizer();
        negativeTotalizer = _flowMeter->getNegativeTotalizer();

        salReturn ret = TSPublisher::SAL()->putSample_flowMeter(this);
        if (ret != SAL__OK) {
            SPDLOG_WARN("Cannot send FlowMeter: {}", ret);
            return;
        }

        runCondition.wait_for(lock, 2s - (std::chrono::steady_clock::now() - start));
    }
    SPDLOG_DEBUG("Flow Meter Thread Stopped.");
}
