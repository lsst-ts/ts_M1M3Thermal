/*
 * Thermal CsC.
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

#if SIMULATOR
#include <SimulatedFPGA.h>
#else
#include <ThermalFPGA.h>
#endif

#include <RIOSubscriber.h>

#include <cRIO/FPGA.h>
#include <cRIO/SALSink.h>

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

void printHelp() {
    std::cout << "M1M3 Thermal System controller. Runs either as simulator or as the real thing on cRIO"
              << std::endl
              << "Options:" << std::endl
              << "  -b runs on background, don't log to console" << std::endl
              << "  -c <configuration path> use given configuration directory (should be SettingFiles)"
              << std::endl
              << "  -d increases debugging (can be specified multiple times, default is info)" << std::endl
              << "  -f runs on foreground, don't log to a file" << std::endl
              << "  -h prints this help" << std::endl
              << "  -s increases SAL debugging (can be specified multiple times, default is 0)" << std::endl;
}

int debugLevel = 0;
int debugLevelSAL = 0;

std::vector<spdlog::sink_ptr> sinks;

void processArgs(int argc, char* const argv[], const char*& configRoot) {
    int enabledSinks = 0x3;

    int opt;
    while ((opt = getopt(argc, argv, "bc:dfhs")) != -1) {
        switch (opt) {
            case 'b':
                enabledSinks &= ~0x01;
                break;
            case 'c':
                configRoot = optarg;
                break;
            case 'd':
                debugLevel++;
                break;
            case 'f':
                enabledSinks &= ~0x02;
                break;
            case 'h':
                printHelp();
                exit(EXIT_SUCCESS);
            case 's':
                debugLevelSAL++;
                break;
            default:
                std::cerr << "Unknow option " << opt << std::endl;
                printHelp();
                exit(EXIT_FAILURE);
        }
    }

    spdlog::init_thread_pool(8192, 1);
    if (enabledSinks & 0x01) {
        auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        sinks.push_back(stdout_sink);
    }
    if (enabledSinks & 0x02) {
        auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("MTM1M3TS", 0, 0);
        sinks.push_back(daily_sink);
    }
    auto logger = std::make_shared<spdlog::async_logger>("MTM1M3TS", sinks.begin(), sinks.end(),
                                                         spdlog::thread_pool(),
                                                         spdlog::async_overflow_policy::block);
    spdlog::set_default_logger(logger);
    spdlog::set_level((debugLevel == 0 ? spdlog::level::info
                                       : (debugLevel == 1 ? spdlog::level::debug : spdlog::level::trace)));
}

void initializeFPGAs(FPGA* fpga) {
    SPDLOG_INFO("Main: Creating fpga");

    fpga->initialize();
    fpga->open();
}

void runFPGAs(std::shared_ptr<SAL_MTM1M3TS> m1m3tsSAL) {
    RIOSubscriber subscriber(m1m3tsSAL);

    try {
        subscriber.start();
        subscriber.join();
    } catch (std::exception& ex) {
        SPDLOG_CRITICAL("Error starting,stopping or joining threads: {)", ex.what());
    }

    subscriber.stop();
#if 0
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
#endif
}

SALSinkMacro(MTM1M3TS);

int main(int argc, char* const argv[]) {
    const char* configRoot = getenv("PWD");

    processArgs(argc, argv, configRoot);

    SPDLOG_INFO("Main: Initializing M1M3TS SAL");
    std::shared_ptr<SAL_MTM1M3TS> m1m3tsSAL = std::make_shared<SAL_MTM1M3TS>();
    m1m3tsSAL->setDebugLevel(debugLevelSAL);

    sinks.push_back(std::make_shared<SALSink_mt>(m1m3tsSAL));
    auto logger = std::make_shared<spdlog::async_logger>("MTM1M3TS", sinks.begin(), sinks.end(),
                                                         spdlog::thread_pool(),
                                                         spdlog::async_overflow_policy::block);
    spdlog::set_default_logger(logger);
    spdlog::level::level_enum logLevel =
            (debugLevel == 0 ? spdlog::level::info
                             : (debugLevel == 1 ? spdlog::level::debug : spdlog::level::trace));
    spdlog::set_level(logLevel);

    SPDLOG_INFO("Main: Creating publisher");
    // M1M3SSPublisher::get().setSAL(m1m3SAL);
    // M1M3SSPublisher::get().newLogLevel(logLevel * 10);

#if SIMULATOR
    FPGA* fpga = new SimulatedFPGA();
#else
    FPGA* fpga = new ThermalFPGA("Bitfiles");
#endif

    try {
        initializeFPGAs(fpga);
        runFPGAs(m1m3tsSAL);

        fpga->close();
        fpga->finalize();
    } catch (NiError& nie) {
        SPDLOG_CRITICAL("Main: Error initializing ThermalFPGA: {}", nie.what());
        fpga->finalize();
        m1m3tsSAL->salShutdown();
        delete fpga;
        return -1;
    }
    SPDLOG_INFO("Main: Shutting down M1M3 TS SAL");
    m1m3tsSAL->salShutdown();

    SPDLOG_INFO("Main: Shutdown complete");

    return 0;
}
