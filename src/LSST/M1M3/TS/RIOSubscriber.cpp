/*
 * RIOSubscriber class.
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

#include <RIOSubscriber.h>

#include <cRIO/Command.h>
#include <SAL_MTM1M3TS.h>
#include <spdlog/spdlog.h>

#include <chrono>

namespace LSST {
namespace M1M3 {
namespace TS {

constexpr int32_t ACK_INPROGRESS = 301;  /// Acknowledges command reception, command is being executed.
constexpr int32_t ACK_COMPLETE = 303;    /// Command is completed.
constexpr int32_t ACK_FAILED = -302;     /// Command execution failed.

RIOSubscriber::RIOSubscriber(std::shared_ptr<SAL_MTM1M3TS> m1m3tsSAL) {
    _events = {"summaryState", "logLevel", "thermalEnabled"};

    for (auto e : _events) {
        m1m3tsSAL->salEventPub((char *)("MTM1M3TS_logevent_" + e).c_str());
    }

    _commands["enable"] = [m1m3tsSAL]() {

    };

    _commands["disable"] = [m1m3tsSAL]() {

    };

    _commands["standby"] = [m1m3tsSAL]() {

    };

    _commands["setLogLevel"] = [m1m3tsSAL]() {
        MTM1M3TS_command_setLogLevelC data;
        MTM1M3TS_logevent_logLevelC newData;
        int32_t commandID = m1m3tsSAL->acceptCommand_setLogLevel(&data);
        if (commandID <= 0) return;

        if (data.level >= 40) {
            spdlog::set_level(spdlog::level::err);
            newData.level = 40;
        } else if (data.level >= 30) {
            spdlog::set_level(spdlog::level::warn);
            newData.level = 30;
        } else if (data.level >= 20) {
            spdlog::set_level(spdlog::level::info);
            newData.level = 20;
        } else if (data.level >= 10) {
            spdlog::set_level(spdlog::level::debug);
            newData.level = 10;
        } else {
            spdlog::set_level(spdlog::level::trace);
            newData.level = 0;
        }
        m1m3tsSAL->ackCommand_setLogLevel(commandID, ACK_COMPLETE, 0, (char *)"Complete");
        m1m3tsSAL->logEvent_logLevel(&newData, 0);
    };

    // register all commands
    for (auto c : _commands) {
        m1m3tsSAL->salProcessor((char *)("MTM1M3TS_command_" + c.first).c_str());
    }
}

RIOSubscriber::~RIOSubscriber() {}

void RIOSubscriber::run() {
    while (keepRunning) {
        tryCommands();
        std::this_thread::sleep_for(100us);
    }
}

void RIOSubscriber::tryCommands() {
    for (auto c : _commands) {
        c.second();
    }
}

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST
