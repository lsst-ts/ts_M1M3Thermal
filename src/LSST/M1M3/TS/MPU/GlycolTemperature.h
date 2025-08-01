/*
 * Glycol Temperature readout
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

#ifndef __TS_MPU_GLYCOLTEMPERATURE__
#define __TS_MPU_GLYCOLTEMPERATURE__

#include <atomic>
#include <vector>

#include <cRIO/MPU.h>
#include <cRIO/Thread.h>
#include <Transports/Transport.h>

namespace LSST {
namespace M1M3 {
namespace TS {

/**
 * Reads glycol temperature measurements. Runs in thread, reading and
 * processing what's available on glycol temperature output.
 */
class GlycolTemperature : public cRIO::MPU, public cRIO::Thread {
public:
    GlycolTemperature(std::shared_ptr<Transports::Transport> transport);

    void run(std::unique_lock<std::mutex>& lock) override;

    /**
     * Called when temperature values are updated.
     */
    virtual void updated() {}

    /**
     * Retrieve array of temperatures.
     *
     * @return array with temperature values
     */
    std::vector<float> getTemperatures();

    /**
     * Retrieves last parsed buffer.
     *
     * @return last parsed buffer
     */
    std::string getDataBuffer();

    /**
     * Number of consecutive failures to receive data from temperature sensor.
     */
    std::atomic<int> receiving_error_count;

private:
    std::shared_ptr<Transports::Transport> _transport;

    std::mutex _temperature_mutex;
    float _temperatures[8];

    std::string _data_buffer;
    std::string _last_processed;
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  // !__TS_MPU_GLYCOLTEMPERATURE__
