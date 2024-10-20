/*
 * Thermal System SAL publisher.
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

#ifndef _TSPublisher_
#define _TSPublisher_

#ifdef SIMULATOR
#include <sys/timex.h>
#endif

#include <SAL_MTM1M3TS.h>

#include <cRIO/Singleton.h>

#include <Telemetry/FlowMeterThread.h>
#include <Telemetry/GlycolTemperatureThread.h>
#include <Telemetry/PumpThread.h>

namespace LSST {
namespace M1M3 {
namespace TS {

class TSPublisher final : public cRIO::Singleton<TSPublisher> {
public:
    TSPublisher(token);
    ~TSPublisher();

    void setSAL(std::shared_ptr<SAL_MTM1M3TS> m1m3TSSAL);

    static std::shared_ptr<SAL_MTM1M3TS> SAL() { return instance()._m1m3TSSAL; }

    void setLogLevel(int newLevel);

    void logSoftwareVersions();
    void logSimulationMode();
    void logThermalInfo(MTM1M3TS_logevent_thermalInfoC *data) { _m1m3TSSAL->logEvent_thermalInfo(data, 0); }

    void startFlowMeterThread();
    void startGlycolTemperatureThread();
    void startPumpThread();

    void stopFlowMeterThread();
    void stopGlycolTemperatureThread();
    void stopPumpThread();

    static double getTimestamp() {
#ifdef SIMULATOR
        struct timex tx;
        struct timespec now;
        double taiTime;

        memset(&tx, 0, sizeof(tx));
        adjtimex(&tx);
        clock_gettime(CLOCK_TAI, &now);
        taiTime = (double)now.tv_sec + (double)now.tv_nsec / 1000000000.;
        return taiTime;
#else
        return instance()._m1m3TSSAL->getCurrentTime();
#endif
    }

private:
    std::shared_ptr<SAL_MTM1M3TS> _m1m3TSSAL;

    MTM1M3TS_logevent_logLevelC _logLevel;

    Telemetry::FlowMeterThread *_flowMeterThread;
    Telemetry::GlycolTemperatureThread *_glycolTemperatureThread;
    Telemetry::PumpThread *_pumpThread;
};

}  // namespace TS
}  // namespace M1M3
}  // namespace LSST

#endif  //! _TSPublisher_
