/*
 * VFD telemetry handling class.
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
#include <Telemetry/VFD.h>

using namespace LSST::M1M3::TS::Telemetry;

VFD::VFD(token) {
    commandedFrequency = NAN;
    targetFrequency = NAN;
    outputFrequency = NAN;
    outputCurrent = NAN;
    busVoltage = NAN;
    outputVoltage = NAN;
}

void VFD::update() {
    auto vfd = IFPGA::get().vfd;
    vfd->poll();

    commandedFrequency = vfd->getCommandedFrequency();
    targetFrequency = vfd->getTargetFrequency();
    outputFrequency = vfd->getOutputFrequency();
    outputCurrent = vfd->getOutputCurrent();
    busVoltage = vfd->getDCBusVoltage();
    outputVoltage = vfd->getOutputVoltage();
}

void VFD::send() {
    salReturn ret = TSPublisher::SAL()->putSample_glycolPump(this);
    if (ret != SAL__OK) {
        SPDLOG_WARN("Cannot send VFD: {}", ret);
        return;
    }
}
