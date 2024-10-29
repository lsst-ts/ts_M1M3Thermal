/*
 * Simulated VFD glycol pump.
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

#ifndef __MPU_SimulatedVFDPump__
#define __MPU_SimulatedVFDPump__

#include <Modbus/Buffer.h>
#include <Transports/SimulatedTransport.h>

namespace LSST {
namespace M1M3 {
namespace TS {

class SimulatedVFDPump : public Transports::SimulatedTransport {
public:
    SimulatedVFDPump();

protected:
    void generate_response(const unsigned char* buf, size_t len) override;

private:
    uint16_t _pump_voltage;
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // !__MPU_SimulatedVFDPump__
