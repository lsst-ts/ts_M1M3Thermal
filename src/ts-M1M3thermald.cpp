/*
 * Thermal CSC.
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

#include <ThermalFPGA.h>

#ifdef SIMULATOR
#include <SimulatedFPGA.h>
#else
#include <ThermalFPGA.h>
#endif

#include <cRIO/ControllerThread.h>
#include <OuterLoopClockThread.h>

#include <TSSubscriber.h>
#include <SALThermalILC.h>
#include <TSPublisher.h>

#include <cRIO/CSC.h>
#include <cRIO/FPGA.h>
#include <cRIO/NiError.h>
#include <cRIO/SALSink.h>

#include <TSApplication.h>

#include <SAL_MTM1M3TS.h>

#include <getopt.h>

#include <chrono>
#include <thread>

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"

using namespace LSST::cRIO;
using namespace LSST::M1M3::TS;

using namespace std::chrono_literals;

class M1M3thermald : public CSC {
public:
    M1M3thermald(std::string name, const char* description) : CSC(name, description) {}

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
    SPDLOG_INFO("Main: Initializing M1M3TS SAL");
    _m1m3tsSAL = std::make_shared<SAL_MTM1M3TS>();
    _m1m3tsSAL->setDebugLevel(getDebugLevelSAL());

    addSink(std::make_shared<SALSink_mt>(_m1m3tsSAL));

    SALThermalILC* ilc = new SALThermalILC(_m1m3tsSAL);

    TSApplication::instance().setILC(ilc);

#ifdef SIMULATOR
    SPDLOG_WARN("Starting Simulator version! Version {}", VERSION);
#else
    SPDLOG_INFO("Starting cRIO/real HW version. Version {}", VERSION);
#endif

    SPDLOG_INFO("Creating publisher");
    TSPublisher::instance().setSAL(_m1m3tsSAL);
    TSPublisher::instance().setLogLevel(getSpdLogLogLevel() * 10);

    SPDLOG_INFO("Starting controller thread");
    ControllerThread::instance().start();
    addThread(new OuterLoopClockThread());

    SPDLOG_INFO("Creating subscriber");
    addThread(new TSSubscriber(_m1m3tsSAL));
}

void M1M3thermald::done() {
    ControllerThread::instance().stop();

    SPDLOG_INFO("Shutting down M1M3thermald");
    removeSink();

    SPDLOG_INFO("Main: Shutting down M1M3 TS SAL");
    _m1m3tsSAL->salShutdown();
}

int M1M3thermald::runLoop() {
    std::this_thread::sleep_for(20ms);
    return 1;
}

#if 0
void runFPGAs(std::shared_ptr<SAL_MTM1M3TS> m1m3tsSAL) {
    SPDLOG_INFO("Main: Creating subscriber");
    M1M3SSSubscriber::get().setSAL(m1m3SAL, mtMountSAL);
    SPDLOG_INFO("Main: Creating subscriber thread");
    SubscriberThread subscriberThread;
    SPDLOG_INFO("Main: Creating outer loop clock thread");
    OuterLoopClockThread outerLoopClockThread;
    SPDLOG_INFO("Main: Creating pps thread");
    PPSThread ppsThread;
    SPDLOG_INFO("Main: Queuing EnterControl command");
    ControllerThread::get().enqueue(CommandFactory::create(Commands::EnterControlCommand));

    try {
        SPDLOG_INFO("Main: Starting pps thread");
        std::thread pps([&ppsThread] { ppsThread.run(); });
        std::this_thread::sleep_for(1500ms);
        SPDLOG_INFO("Main: Starting subscriber thread");
        std::thread subscriber([&subscriberThread] { subscriberThread.run(); });
        SPDLOG_INFO("Main: Starting controller thread");
        std::thread controller([] { ControllerThread::get().run(); });
        SPDLOG_INFO("Main: Starting outer loop clock thread");
        std::thread outerLoopClock([&outerLoopClockThread] { outerLoopClockThread.run(); });

        SPDLOG_INFO("Main: Waiting for ExitControl");
        Model::get().waitForExitControl();
        SPDLOG_INFO("Main: ExitControl received");

        SPDLOG_INFO("Main: Stopping pps thread");
        ppsThread.stop();
        SPDLOG_INFO("Main: Stopping subscriber thread");
        subscriberThread.stop();
        SPDLOG_INFO("Main: Stopping controller thread");
        ControllerThread::get().stop();
        SPDLOG_INFO("Main: Stopping outer loop clock thread");
        outerLoopClockThread.stop();
        std::this_thread::sleep_for(100ms);
        SPDLOG_INFO("Main: Joining pps thread");
        pps.join();
        SPDLOG_INFO("Main: Joining subscriber thread");
        subscriber.join();
        SPDLOG_INFO("Main: Joining controller thread");
        controller.join();
        SPDLOG_INFO("Main: Joining outer loop clock thread");
        outerLoopClock.join();
    } catch (std::exception& ex) {
        SPDLOG_CRITICAL("Error starting.stopping or joining threads: {)", ex.what());
        exit(1);
    }
}
#endif

int main(int argc, char* const argv[]) {
    M1M3thermald csc("M1M3TS", "M1M3 Thermal System CSC");

    csc.processArgs(argc, argv);

#ifdef SIMULATOR
    SimulatedFPGA* fpga = new SimulatedFPGA();
#else
    ThermalFPGA* fpga = new ThermalFPGA("Bitfiles");
#endif

    TSApplication::instance().setFPGA(fpga);

    try {
        csc.run(fpga);
    } catch (NiError& nie) {
        SPDLOG_CRITICAL("Main: Error initializing ThermalFPGA: {}", nie.what());
    }

    delete fpga;

    SPDLOG_INFO("Main: Shutdown complete");

    return 0;
}
