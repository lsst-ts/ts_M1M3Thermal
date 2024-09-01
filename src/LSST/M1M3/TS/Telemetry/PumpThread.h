/*
 * VFD/pump motor controller telemetry handling class.
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

#ifndef _TS_Telemetry_PumpThread_
#define _TS_Telemetry_PumpThread_

#include <SAL_MTM1M3TS.h>

#include <cRIO/Thread.h>

#include <MPU/VFD.h>

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Telemetry {

/**
 * Thread reading out pump values. Started from TSPublisher when CSC
 * enteres disabled state, updates SAL VFD pump telemetery.
 */
class PumpThread final : public cRIO::Thread, MTM1M3TS_glycolPumpC {
public:
    PumpThread(std::shared_ptr<VFD> vfd);

    void run(std::unique_lock<std::mutex>& lock) override;

private:
    std::shared_ptr<VFD> _vfd;
};

}  // namespace Telemetry
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // !_TS_Telemetry_VFD_
