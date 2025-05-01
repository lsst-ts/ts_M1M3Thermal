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

#ifndef _TS_Telemetry_GlycolLoopTemperature_
#define _TS_Telemetry_GlycolLoopTemperature_

#include <mutex>
#include <vector>

#include <SAL_MTM1M3TS.h>
#include <cRIO/Singleton.h>

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Telemetry {

/**
 * Class holding and publishing glycol loop temperature telemetry. A singleton,
 * from which various glycol loop temperatures can be accessed.
 */
class GlycolLoopTemperature final : MTM1M3TS_glycolLoopTemperatureC,
                                    public cRIO::Singleton<GlycolLoopTemperature> {
public:
    GlycolLoopTemperature(token);

    /**
     * Retrieves values from parsed float array, set internal values, and sends
     * updated telemetry through SAL/DDS.
     *
     * @param temperatures 8 position array with temperature values
     */
    void update(const std::vector<float> &temperatures);

    /**
     * Retrieves last air temperature, measured above the mirror
     *
     * @return above mirror air temperature
     */
    float getAboveMirrorTemperature();

    /**
     * Retrieves average temperature in the mirror cell.
     *
     * @return average temperature of three sensors in the mirror cell
     */
    float getMirrorCellInsideTemperature();

    /**
     * Retrieves weighted average mirror glycol loop temperature.
     *
     * @param supply supply contribution, 0 to 1
     *
     * @return average of the supply and return values
     */
    float getMirrorLoopAverage(float supply);

    /**
     * Returns mirror glycol supply temperature.
     */
    float getMirrorLoopSupply();

    /**
     * Returns mirror glycol return temperature.
     */
    float getMirrorLoopReturn();

    /**
     * Returns telescope glycol loop supply temperature.
     */
    float getTelescopeLoopSupply();

    /**
     * Returns telescope glycol loop return temperature.
     */
    float getTelescopeLoopReturn();

private:
    std::mutex _access_mutex;
};

}  // namespace Telemetry
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // !_TS_Telemetry_GlycolLoopTemperature_
