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

PumpThread::PumpThread(std::shared_ptr<Transports::Transport> transport) {
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
    int error_count = 0;

    SPDLOG_INFO("Running Pump Thread.");
    while (keepRunning) {
        auto end = std::chrono::steady_clock::now() + 2s;

        try {
            vfd.clear();

            switch (_next_request) {
                case START:
                    vfd.start();
                    break;
                case STOP:
                    vfd.stop();
                    break;
                case RESET:
                    vfd.reset();
                    break;
                case FREQ:
                    vfd.setFrequency(_target_frequency);
                    break;
                case NOP:
                    break;
            }

            if (_next_request != NOP) {
                _transport->commands(vfd, 2s, this);
                _next_request = NOP;
            }

            vfd.readInfo();

            _transport->commands(vfd, 2s, this);

            Events::GlycolPumpStatus::instance().update(&vfd);

            commandedFrequency = vfd.getCommandedFrequency();
            targetFrequency = vfd.getTargetFrequency();
            outputFrequency = vfd.getOutputFrequency();
            outputCurrent = vfd.getOutputCurrent();
            busVoltage = vfd.getDCBusVoltage();
            outputVoltage = vfd.getOutputVoltage();

            salReturn ret = TSPublisher::SAL()->putSample_glycolPump(this);
            if (ret != SAL__OK) {
                SPDLOG_WARN("Cannot send VFD: {}", ret);
            }
            error_count = 0;
        } catch (std::runtime_error& er) {
            if (error_count == 0) {
                SPDLOG_ERROR("Error in running Glycol Pump thread: {}", er.what());
            }
            error_count++;
            std::this_thread::sleep_for(2s);
            _transport->flush();
        }

        runCondition.wait_until(lock, end);
    }

    SPDLOG_DEBUG("Pump Thread Stopped.");
}

void PumpThread::start_pump() { _next_request = START; }

void PumpThread::stop_pump() { _next_request = STOP; }
void PumpThread::reset_pump() { _next_request = RESET; }
void PumpThread::set_target_frequency(float frequency) {
    _target_frequency = frequency;
    _next_request = FREQ;
}
