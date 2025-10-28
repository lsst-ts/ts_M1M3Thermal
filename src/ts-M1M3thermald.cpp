/*
 * Thermal CSC.
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

#include <csignal>
#include <getopt.h>

#include <chrono>
#include <thread>

#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog/async.h>
#include <spdlog/spdlog.h>

#include <SAL_MTM1M3TS.h>

#include <cRIO/CSC.h>
#include <cRIO/ControllerThread.h>
#include <cRIO/FPGA.h>
#include <cRIO/NiError.h>
#include <cRIO/SALSink.h>
#include <cRIO/Settings/Path.h>

#include <OuterLoopClockThread.h>
#include <Settings/Controller.h>

#include "Commands/EnterControl.h"
#include "Commands/ReloadConfiguration.h"
#include "Commands/SAL.h"
#include "Events/SummaryState.h"
#include "SALThermalILC.h"
#include "TSApplication.h"
#include "TSPublisher.h"
#include "TSSubscriber.h"

using namespace std::chrono_literals;
using namespace LSST::M1M3::TS;

void sig_usr1(int signal) {
    LSST::cRIO::ControllerThread::instance().enqueue(std::make_shared<Commands::ReloadConfiguration>());
}

extern const char *VERSION;

class M1M3thermald : public LSST::cRIO::CSC {
public:
    M1M3thermald(const char *name, const char *description) : CSC(name, description) {}

protected:
    void init() override;

    void done() override;

    int runLoop() override;

private:
    std::shared_ptr<SAL_MTM1M3TS> _m1m3tsSAL;
    std::unique_ptr<TSSubscriber> _subscriber;
};

SALSinkMacro(MTM1M3TS);

void M1M3thermald::init() {
    SPDLOG_INFO("Setting root path {}", getConfigRoot());
    LSST::cRIO::Settings::Path::setRootPath(getConfigRoot());

    SPDLOG_INFO("Initializing M1M3TS SAL");
    try {
        _m1m3tsSAL = std::make_shared<SAL_MTM1M3TS>();
    } catch (std::runtime_error &er) {
        SPDLOG_CRITICAL("Cannot initialize SAL: {}", er.what());
        throw er;
    }

    _m1m3tsSAL->setDebugLevel(getDebugLevelSAL());

    addSink(std::make_shared<SALSink_mt>(_m1m3tsSAL));

    SALThermalILC *ilc = new SALThermalILC(_m1m3tsSAL);

    TSApplication::instance().setILC(ilc);

#ifdef SIMULATOR
    SPDLOG_WARN("Starting Simulator version! Version {}", VERSION);
#else
    SPDLOG_INFO("Starting cRIO/real HW version. Version {}", VERSION);
#endif

    SPDLOG_INFO("Creating publisher");
    TSPublisher::instance().setSAL(_m1m3tsSAL);
    TSPublisher::instance().setLogLevel(static_cast<int>(getSpdLogLogLevel()) * 10);

    TSPublisher::instance().startGlycolTemperatureThread();

    SPDLOG_INFO("Starting controller thread");
    LSST::cRIO::ControllerThread::instance().start(500ms);
    addThread(new OuterLoopClockThread());

    SPDLOG_INFO("Creating subscriber");
    addThread(new TSSubscriber(_m1m3tsSAL));

    LSST::cRIO::ControllerThread::instance().enqueue(std::make_shared<Commands::EnterControl>());

    signal(SIGUSR1, sig_usr1);

    daemonOK();
}

void M1M3thermald::done() {
    Events::SummaryState::set_state(MTM1M3TS::MTM1M3TS_shared_SummaryStates_OfflineState);

    LSST::cRIO::ControllerThread::instance().stop();
    TSPublisher::instance().stopFlowMeterThread();
    TSPublisher::instance().stopPumpThread();

    SPDLOG_INFO("Shutting down M1M3thermald");
    removeSink();

    std::this_thread::sleep_for(10ms);

    SPDLOG_INFO("Main: Shutting down M1M3 TS SAL");
    _m1m3tsSAL->salShutdown();
}

int M1M3thermald::runLoop() {
    std::this_thread::sleep_for(20ms);
    return LSST::cRIO::ControllerThread::exitRequested() ? 0 : 1;
}

int main(int argc, char *const argv[]) {
    M1M3thermald csc("M1M3TS", "M1M3 Thermal System CSC");

    csc.processArgs(argc, argv);

    try {
        csc.run(&IFPGA::get());
    } catch (LSST::cRIO::NiError &nie) {
        SPDLOG_CRITICAL("Main: Error initializing ThermalFPGA: {}", nie.what());
    }

    return 0;
}
