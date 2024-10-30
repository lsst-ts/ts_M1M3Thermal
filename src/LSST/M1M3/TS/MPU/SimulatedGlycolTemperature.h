/*
 * Simulated glycol thermocouples device.
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

#ifndef __MPU_SimulatedGlycolTemperature__
#define __MPU_SimulatedGlycolTemperature__

#include <random>

#include <Modbus/Buffer.h>
#include <Transports/Transport.h>

namespace LSST {
namespace M1M3 {
namespace TS {

class SimulatedGlycolTemperature : public Transports::Transport {
public:
    SimulatedGlycolTemperature();

    void write(const unsigned char* buf, size_t len) override;
    std::vector<uint8_t> read(size_t len, std::chrono::microseconds timeout,
                              LSST::cRIO::Thread* calling_thread = NULL) override;

    void commands(Modbus::BusList& bus_list, std::chrono::microseconds timeout,
                  LSST::cRIO::Thread* calling_thread = NULL) override;

    void flush() override;
    void telemetry(uint64_t& write_bytes, uint64_t& read_bytes) override;

private:
    uint64_t _bytes_written;
    uint64_t _bytes_read;

    float _temperatures[8];

    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution;
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // ! __MPU_SimulatedGlycolTemperature__
