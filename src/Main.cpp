/******************************************************************************
 * This file is part of MTM1M3TS.
 *
 * Developed for the LSST Telescope and Site Systems.
 * This product includes software developed by the LSST Project
 * (https://www.lsst.org).
 * See the COPYRIGHT file at the top-level directory of this distribution
 * for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *****************************************************************************/

#include <iostream>
#include <thread>
#include <unistd.h>

#include "InterlockSystem/InterlockSystem.h"
#include "MainSystem/Controller.h"
#include "MainSystem/MainSystem.h"
#include "Settings/Settings.h"
#include "Threads/ControllerThread.h"
#include "Threads/SubscriberThread.h"
#include "Utility/Logger.h"
#include "Utility/MTM1M3TS.h"

LSST::TS::MTM1M3TS::Logger Log;

using namespace LSST::TS::MTM1M3TS;

void run(IThread thread) { thread.run(); }

int main() {
    Log.SetLevel(LoggerLevels::Info);
    Log.Info("Main: Starting MTM1M3TS Software");

    Log.Info("Main: Initializing Settings");
    Settings settings = Settings();

    Log.Info("Main: Initializing SAL_MTM1M3TS");
    SAL_MTM1M3TS mtm1m3ts = SAL_MTM1M3TS();
    initializeMTM1M3TS(mtm1m3ts);

    Log.Info("Main: Initializing Thermal FPGA System");
    ThermalFPGASystemData thermalFPGAData;

    Log.Info("Main: Initializing Interlock System");
    InterlockSystemData interlockSystemData;
    InterlockSystemSettings interlockSystemSettings = settings.getInterlockSystemSettings();
    InterlockSystemModel interlockSystemModel = InterlockSystemModel(interlockSystemSettings, interlockSystemData, thermalFPGAData);
    InterlockSystemContext interlockSystemContext = InterlockSystemContext(interlockSystemModel);
    InterlockSystem interlockSystem = InterlockSystem(interlockSystemContext);

    Log.Info("Main: Initializing Main System");
    MainSystemData mainSystemData;
    MainSystemSettings mainSystemSettings = settings.getMainSystemSettings();
    MainSystemModel mainSystemModel = MainSystemModel(mainSystemSettings, mainSystemData, interlockSystem);
    MainSystemContext mainSystemContext = MainSystemContext(mainSystemModel);
    MainSystem mainSystem = MainSystem(mainSystemContext);

    Log.Info("Main: Initializing Controller");
    Controller controller = Controller(mainSystem);
    ControllerThread controllerThreadObject = ControllerThread(controller);
    std::thread controllerThread = std::thread(&ControllerThread::run, &controllerThreadObject);

    Log.Info("Main: Initializing Subscriber");
    SubscriberThread subscriberThreadObject = SubscriberThread(mtm1m3ts, controller);
    std::thread subscriberThread = std::thread(&SubscriberThread::run, &subscriberThreadObject);

    Log.Info("Main: Finalizing Initialization");
    controller.lock();
    controller.enqueue(new BootCommand());
    controller.unlock();
    usleep(100000);

    Log.Info("Main: Initialization Complete");
    mainSystemModel.waitForShutdown();

    Log.Info("Main: Shutting down Subscriber");
    subscriberThreadObject.stop();
    subscriberThread.join();

    Log.Info("Main: Shutting down Controller");
    controllerThreadObject.stop();
    controllerThread.join();

    Log.Info("Main: Shutting down SAL_MTM1M3TS");
    usleep(100000);
    mtm1m3ts.salShutdown();

    Log.Info("Main: Shutdown Complete");

    return 0;
}