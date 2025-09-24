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

#include <spdlog/spdlog.h>

#ifdef SIMULATOR
#include "MPU/SimulatedFlowMeter.h"
#include "MPU/SimulatedGlycolTemperature.h"
#include "MPU/SimulatedVFDPump.h"
#else
#include "Transports/FPGASerialDevice.h"
#include "ThermalFPGA.h"
#endif

#include "NiFpga/NiFpga_ts_M1M3ThermalFPGA.h"

#include "IFPGA.h"
#include "TSPublisher.h"

using namespace LSST::M1M3::TS;

extern const char *VERSION;

TSPublisher::TSPublisher(token) {
    _logLevel.level = -1;

    _flowMeterThread = NULL;
    pump_thread = NULL;
    _glycolTemperatureThread = NULL;
}

TSPublisher::~TSPublisher() {
    stopFlowMeterThread();
    stopPumpThread();
}

void TSPublisher::setSAL(std::shared_ptr<SAL_MTM1M3TS> m1m3TSSAL) {
    _m1m3TSSAL = m1m3TSSAL;

    SPDLOG_DEBUG("TSPublisher: Initializing SAL Telemetry");
    _m1m3TSSAL->salTelemetryPub((char *)"MTM1M3TS_thermalData");
    _m1m3TSSAL->salTelemetryPub((char *)"MTM1M3TS_mixingValve");
    _m1m3TSSAL->salTelemetryPub((char *)"MTM1M3TS_glycolLoopTemperature");
    _m1m3TSSAL->salTelemetryPub((char *)"MTM1M3TS_flowMeter");
    _m1m3TSSAL->salTelemetryPub((char *)"MTM1M3TS_glycolPump");

    SPDLOG_DEBUG("TSPublisher: Initializing SAL Events");
    _m1m3TSSAL->salEventPub((char *)"MTM1M3TS_logevent_appliedSetpoints");
    _m1m3TSSAL->salEventPub((char *)"MTM1M3TS_logevent_engineeringMode");
    _m1m3TSSAL->salEventPub((char *)"MTM1M3TS_logevent_enabledILC");
    _m1m3TSSAL->salEventPub((char *)"MTM1M3TS_logevent_errorCode");
    _m1m3TSSAL->salEventPub((char *)"MTM1M3TS_logevent_fcuTargets");
    _m1m3TSSAL->salEventPub((char *)"MTM1M3TS_logevent_heartbeat");
    _m1m3TSSAL->salEventPub((char *)"MTM1M3TS_logevent_logLevel");
    _m1m3TSSAL->salEventPub((char *)"MTM1M3TS_logevent_summaryState");
    _m1m3TSSAL->salEventPub((char *)"MTM1M3TS_logevent_softwareVersions");
    _m1m3TSSAL->salEventPub((char *)"MTM1M3TS_logevent_simulationMode");

    _m1m3TSSAL->salEventPub((char *)"MTM1M3TS_logevent_thermalInfo");
    _m1m3TSSAL->salEventPub((char *)"MTM1M3TS_logevent_thermalWarning");
    _m1m3TSSAL->salEventPub((char *)"MTM1M3TS_logevent_glycolPumpStatus");

    _m1m3TSSAL->salEventPub((char *)"MTM1M3TS_logevent_thermalSettings");
}

void TSPublisher::setLogLevel(int newLevel) {
    if (_logLevel.level != newLevel) {
        SPDLOG_TRACE("logEvent_logLevel {}", newLevel);
        _logLevel.level = newLevel;
        _m1m3TSSAL->logEvent_logLevel(&_logLevel, 0);
    }
}

void TSPublisher::logSoftwareVersions() {
    MTM1M3TS_logevent_softwareVersionsC versions;
    versions.salVersion = SAL_MTM1M3TS::getSALVersion();
    versions.xmlVersion = SAL_MTM1M3TS::getXMLVersion();
    versions.cscVersion = VERSION;
    versions.subsystemVersions = "";
    _m1m3TSSAL->logEvent_softwareVersions(&versions, 0);
}

void TSPublisher::logSimulationMode() {
    MTM1M3TS_logevent_simulationModeC simulation;
#ifdef SIMULATOR
    simulation.mode = 1;
#else
    simulation.mode = 0;
#endif
    _m1m3TSSAL->logEvent_simulationMode(&simulation, 0);
}

void TSPublisher::startFlowMeterThread() {
    delete _flowMeterThread;
#ifdef SIMULATOR
    _flowMeterThread = new Telemetry::FlowMeterThread(std::make_shared<SimulatedFlowMeter>());
#else
    _flowMeterThread = new Telemetry::FlowMeterThread(std::make_shared<Transports::FPGASerialDevice>(
            dynamic_cast<ThermalFPGA *>(&IFPGA::get())->getSession(),
            NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_FlowMeterWrite,
            NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_FlowMeterRead, 100ms));
#endif
    _flowMeterThread->start();
}

void TSPublisher::startGlycolTemperatureThread() {
    delete _glycolTemperatureThread;
#ifdef SIMULATOR
    _glycolTemperatureThread =
            new Telemetry::GlycolTemperatureThread(std::make_shared<SimulatedGlycolTemperature>());
#else
    _glycolTemperatureThread =
            new Telemetry::GlycolTemperatureThread(std::make_shared<Transports::FPGASerialDevice>(
                    dynamic_cast<ThermalFPGA *>(&IFPGA::get())->getSession(),
                    NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_CoolantTempWrite,
                    NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_CoolantTempRead, 1ms));
#endif
    _glycolTemperatureThread->start();
}

void TSPublisher::startPumpThread() {
    if (pump_thread != NULL) {
        pump_thread->stop();
        delete pump_thread;
        pump_thread = NULL;
    }
#ifdef SIMULATOR
    pump_thread = new Telemetry::PumpThread(std::make_shared<SimulatedVFDPump>());
#else
    pump_thread = new Telemetry::PumpThread(std::make_shared<Transports::FPGASerialDevice>(
            dynamic_cast<ThermalFPGA *>(&IFPGA::get())->getSession(),
            NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_GlycoolWrite,
            NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_GlycoolRead, 10ms));
#endif
    pump_thread->start();
}

void TSPublisher::startupPump() { pump_thread->startup(); }

void TSPublisher::stopFlowMeterThread() {
    if (_flowMeterThread == NULL) {
        return;
    }

    _flowMeterThread->stop();
    delete _flowMeterThread;
    _flowMeterThread = NULL;
}

void TSPublisher::stopGlycolTemperatureThread() {
    if (_glycolTemperatureThread == NULL) {
        return;
    }

    _glycolTemperatureThread->stop();
    delete _glycolTemperatureThread;
    _glycolTemperatureThread = NULL;
}

void TSPublisher::stopPumpThread() {
    if (pump_thread == NULL) {
        return;
    }

    pump_thread->stop();
    delete pump_thread;
    pump_thread = NULL;
}
