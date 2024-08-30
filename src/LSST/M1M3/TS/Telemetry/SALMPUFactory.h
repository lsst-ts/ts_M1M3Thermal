/*
 * Interface for factory creating application specific MPU objects.
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

#ifndef __TS_SALMPUFactory__
#define __TS_SALMPUFactory__

#include <MPU/FactoryInterface.h>

#include <MPU/FlowMeter.h>
#include "VFDSAL.h"

namespace LSST {
namespace M1M3 {
namespace TS {
namespace Telemetry {

class SALMPUFactory : public FactoryInterface {
public:
    std::shared_ptr<FlowMeter> createFlowMeter() override { return std::make_shared<FlowMeter>(2); }
    std::shared_ptr<VFD> createVFD() override { return std::make_shared<VFDSAL>(1); }
};

}  // namespace Telemetry
}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif /* __TS_SALMPUFactory__ */
