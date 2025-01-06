/*
 * GlycolLoopTemperature telemetry handling class.
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

#include <Telemetry/GlycolLoopTemperature.h>
#include <TSPublisher.h>

using namespace LSST::M1M3::TS::Telemetry;

GlycolLoopTemperature::GlycolLoopTemperature(token) {
    aboveMirrorTemperature = NAN;
    insideCellTemperature1 = NAN;
    insideCellTemperature2 = NAN;
    insideCellTemperature3 = NAN;
    telescopeCoolantSupplyTemperature = NAN;
    telescopeCoolantReturnTemperature = NAN;
    mirrorCoolantSupplyTemperature = NAN;
    mirrorCoolantReturnTemperature = NAN;
}

void GlycolLoopTemperature::update(const std::vector<float> &temperatures) {
    {
        const std::lock_guard<std::mutex> lock(_access_mutex);

        aboveMirrorTemperature = temperatures[0];
        insideCellTemperature1 = temperatures[1];
        insideCellTemperature2 = temperatures[2];
        insideCellTemperature3 = temperatures[3];
        telescopeCoolantSupplyTemperature = temperatures[4];
        telescopeCoolantReturnTemperature = temperatures[5];
        mirrorCoolantSupplyTemperature = temperatures[6];
        mirrorCoolantReturnTemperature = temperatures[7];
    }

    salReturn ret = TSPublisher::SAL()->putSample_glycolLoopTemperature(this);
    if (ret != SAL__OK) {
        SPDLOG_WARN("Cannot send GlycolTemperatureThread: {}", ret);
        return;
    }
}

float GlycolLoopTemperature::getAirTemperature() {
    std::lock_guard<std::mutex> lock(_access_mutex);
    return (insideCellTemperature1 + insideCellTemperature2 + insideCellTemperature3) / 3.0;
}
