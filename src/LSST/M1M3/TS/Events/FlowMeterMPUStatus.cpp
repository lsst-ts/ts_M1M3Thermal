/*
 * Publish MPU Flow Meter Telemetry.
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

#include <chrono>

#include <spdlog/spdlog.h>

#include <Events/FlowMeterMPUStatus.h>
#include <IFPGA.h>
#include <TSPublisher.h>

using namespace LSST::M1M3::TS;
using namespace LSST::M1M3::TS::Events;

using namespace std::chrono_literals;

FlowMeterMPUStatus::FlowMeterMPUStatus() {}

void FlowMeterMPUStatus::run(std::unique_lock<std::mutex>& lock) {
    SPDLOG_INFO("Running flow meter");
    while (keepRunning) {
        auto start = std::chrono::steady_clock::now();

        auto flowMeter = IFPGA::get().flowMeter;

        flowMeter->clear();
        flowMeter->readHoldingRegisters(1000, 4, 255);
        IFPGA::get().mpuCommands(*flowMeter);

        flowMeter->clear();
        flowMeter->readHoldingRegisters(2500, 6, 255);
        IFPGA::get().mpuCommands(*flowMeter);

        flowMeter->clear();
        flowMeter->readHoldingRegisters(5500, 1, 255);
        IFPGA::get().mpuCommands(*flowMeter);

        SPDLOG_INFO("Sending FlowMeterMPUStatus");

        send();

        runCondition.wait_for(lock, 2s - (std::chrono::steady_clock::now() - start));
    }
}

void FlowMeterMPUStatus::send() {
    salReturn ret = TSPublisher::SAL()->putSample_logevent_flowMeterMPUStatus(this);
    if (ret != SAL__OK) {
        SPDLOG_WARN("Cannot send flowMeterMPUStatus: {}", ret);
        return;
    }
}
