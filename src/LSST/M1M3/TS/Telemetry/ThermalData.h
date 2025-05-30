/*
 * ThermalData telemetry handling class.
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

#ifndef _TS_Telemetry_ThermalData_
#define _TS_Telemetry_ThermalData_

#include <SAL_MTM1M3TS.h>
#include <cRIO/Singleton.h>

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Telemetry {

class ThermalData final : MTM1M3TS_thermalDataC, public cRIO::Singleton<ThermalData> {
public:
    ThermalData(token);

    /**
     * Resets stored values to NAN/0.
     */
    void reset();

    void update(uint8_t address, uint8_t status, float differentialTemperature, uint8_t fanRPM,
                float absoluteTemperature);

    /**
     * Sends updates through SAL/DDS.
     */
    void send();

    bool is_heater_disabled(int index) { return heaterDisabled[index]; }

    auto get_absoluteTemperature() { return absoluteTemperature; }
};

}  // namespace Telemetry
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // !_TS_Telemetry_ThermalData_
