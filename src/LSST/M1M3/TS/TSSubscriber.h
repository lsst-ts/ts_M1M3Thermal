/*
 * TSSubscriber class.
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

#ifndef _TS_TSSubscriber_
#define _TS_TSSubscriber_

#include <cRIO/ControllerThread.h>
#include <cRIO/Thread.h>

#include <SAL_MTM1M3TS.h>
#include <functional>
#include <map>
#include <memory>

namespace LSST {
namespace M1M3 {
namespace TS {

/**
 * Subscribes to SAL events. Looks for commands.
 */
class TSSubscriber : public cRIO::Thread {
public:
    /**
     * Subscribes method calls and extra telemetry.
     */
    TSSubscriber(std::shared_ptr<SAL_MTM1M3TS> m1m3tsSAL);
    virtual ~TSSubscriber();

protected:
    void run(std::unique_lock<std::mutex> &lock) override;

private:
    std::vector<std::string> _events;
    std::map<std::string, std::function<void(void)>> _commands;

    void tryCommands();
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif
