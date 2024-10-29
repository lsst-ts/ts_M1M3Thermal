/*
 * VFD telemetry handling class.
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

#include <Events/GlycolPumpStatus.h>
#include <IFPGA.h>
#include <TSPublisher.h>
#include <Telemetry/PumpThread.h>

using namespace LSST::M1M3::TS::Telemetry;
using namespace std::chrono_literals;

PumpThread::PumpThread(std::shared_ptr<VFD> vfd, std::shared_ptr<Transports::Transport> transport) {
    _vfd = vfd;
    _transport = transport;
    commandedFrequency = NAN;
    targetFrequency = NAN;
    outputFrequency = NAN;
    outputCurrent = NAN;
    busVoltage = NAN;
    outputVoltage = NAN;
}

void PumpThread::run(std::unique_lock<std::mutex>& lock) {
    runCondition.wait_for(lock, std::chrono::seconds(3));

    SPDLOG_INFO("Running Pump Thread.");
    while (keepRunning) {
        auto end = std::chrono::steady_clock::now() + 2s;

        _vfd->readInfo();

        _transport->commands(*_vfd, 2s, this);

        commandedFrequency = _vfd->getCommandedFrequency();
        targetFrequency = _vfd->getTargetFrequency();
        outputFrequency = _vfd->getOutputFrequency();
        outputCurrent = _vfd->getOutputCurrent();
        busVoltage = _vfd->getDCBusVoltage();
        outputVoltage = _vfd->getOutputVoltage();

        salReturn ret = TSPublisher::SAL()->putSample_glycolPump(this);
        if (ret != SAL__OK) {
            SPDLOG_WARN("Cannot send VFD: {}", ret);
            return;
        }

        runCondition.wait_until(lock, end);
    }

    SPDLOG_DEBUG("Pump Thread Stopped.");
}
