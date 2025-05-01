/*
 * Publish MPU Glycol Pump status.
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

using namespace LSST::M1M3::TS::Events;

GlycolPumpStatus::GlycolPumpStatus(token) {
    _last_status = 0xFFFF;
    _last_errorCode = 0xFFFF;
}

void GlycolPumpStatus::update(VFD *vfd) {
    auto status = vfd->getStatus();
    auto errorCode = vfd->getDriveErrorCodes();
    if (status != _last_status || errorCode != _last_errorCode) {
        // bits are from VFD manual
        ready = status & 0x0001;
        running = status & 0x0002;
        forwardCommanded = status & 0x0004;
        forwardRotating = status & 0x0008;
        accelerating = status & 0x0010;
        decelerating = status & 0x0020;
        // 0x0040 is unused
        faulted = status & 0x0080;
        mainFrequencyControlled = status & 0x0100;
        operationCommandControlled = status & 0x0200;
        parametersLocked = status & 0x0400;
        errorCode = status & 0x0800;

        salReturn ret = TSPublisher::SAL()->putSample_logevent_glycolPumpStatus(&instance());
        if (ret != SAL__OK) {
            SPDLOG_WARN("Can not send GlycolPumpStatus: {}", ret);
            return;
        }

        _last_status = status;
        _last_errorCode = errorCode;
    }
}
