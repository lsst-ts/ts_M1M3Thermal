/*
 * Subscriber class.
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

#ifndef _TS_Subscriber_
#define _TS_Subscriber_

#include <cRIO/ControllerThread.h>

#include <SAL_MTM1M3TS.h>
#include <functional>
#include <map>
#include <memory>

namespace LSST {
namespace M1M3 {
namespace TS {

class SALCommand {
public:
    virtual ~SALCommand() {}
    virtual void tryAccept(void *sal) = 0;
};

#define CREATE_SAL_COMMAND_CLASS(SAL, command)                                                               \
    class SAL##_command_##command : public SALCommand {                                                      \
    public:                                                                                                  \
        SAL##_command_##command() {}                                                                         \
        virtual void tryAccept(void *sal) override {                                                         \
            int32_t commandID = reinterpret_cast<SAL_##SAL *>(sal)->acceptCommand_##command(&command##Data); \
            if (commandID > 0) {                                                                             \
                cRIO::ControllerThread::instance().enqueue(new Command_##command(commandID, command##Data)); \
            }                                                                                                \
        }                                                                                                    \
                                                                                                             \
    private:                                                                                                 \
        SAL##_command_##command##C command##Data;                                                            \
    };  // namespace M1M3

/**
 * Subscribes to SAL events. Looks for commands.
 */
class Subscriber {
public:
    /**
     * Subscribes method calls and extra telemetry.
     */
    Subscriber(std::shared_ptr<SAL_MTM1M3TS> SAL);
    virtual ~Subscriber();

    void tryCommands();

private:
    std::map<std::string, std::function<void(void)>> _commands;
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif
