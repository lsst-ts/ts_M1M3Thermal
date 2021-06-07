/*
 * ThermalData telemetry handling class.
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

#include <cRIO/ThermalILC.h>
#include <TSPublisher.h>
#include <Telemetry/ThermalData.h>
#include <spdlog/spdlog.h>

#include <cmath>

using namespace LSST::M1M3::TS;
using namespace LSST::M1M3::TS::Telemetry;

ThermalData::ThermalData(token) {
    for (int i = 0; i < LSST::cRIO::NUM_TS_ILC; i++) {
        ilcFault[i] = false;
        heaterDisabled[i] = true;
        heaterBreaker[i] = true;
        fanBreaker[i] = true;
        differentialTemperature[i] = NAN;
        fanRPM[i] = -1;
        absoluteTemperature[i] = NAN;
    }
}

void ThermalData::update(uint8_t address, uint8_t _status, float _differentialTemperature, uint8_t _fanRPM,
                         float _absoluteTemperature) {
    uint8_t index = address - 1;
    ilcFault[index] = _status & 0x01;
    heaterDisabled[index] = _status & 0x02;
    heaterBreaker[index] = _status & 0x04;
    fanBreaker[index] = _status & 0x08;
    differentialTemperature[index] = _differentialTemperature;
    fanRPM[index] = (int)_fanRPM * 10.0;
    absoluteTemperature[index] = _absoluteTemperature;
}

void ThermalData::send() {
    salReturn ret = TSPublisher::SAL()->putSample_thermalData(this);
    if (ret != SAL__OK) {
        SPDLOG_WARN("Cannot send thermalData: {}", ret);
        return;
    }
}
